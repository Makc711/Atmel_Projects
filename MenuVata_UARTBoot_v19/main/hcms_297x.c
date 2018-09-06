/* Основные функции для работы с модулем HCMS-29xx
 * Большинство низкоуровневых функций сделаны локальными, но ничто не мешает
 * сделать их публичными.
 */

#include "hcms_297x.h"

// Макрос для совместимости кода с новыми версиями GCC, поддерживающими __flash
#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 3))
#define p_get(x)	*(x)
#else
#define __flash		PROGMEM
#define p_get(x)	pgm_read_byte(x)
#endif

// Шрифт надо включать именно после предыдущего макроса, иначе будет ошибка
#include "hcms_297x_font5x7.h"

char hcms_screen[SCR_SIZE]; // Экранный символьный буфер (SCR_SIZE - количество символов на дисплее)

#if defined(USE_CURSOR)
static uint8_t cursor_pos = NO_CURSOR;
#endif

// Управляющие регистры:
static ctrl_reg0_t def_ctrl_r0;
static ctrl_reg1_t def_ctrl_r1;


#define hcms_ctrl_mode()	PORT(PORT_RS) |= _BV(PIN_RS)  // Макрос включения режима записи в регистры управления | 1 на RS
#define hcms_data_mode()	PORT(PORT_RS) &= ~_BV(PIN_RS) // Макрос включения режима записи данных	              | 0 на RS
#define hcms_enable()		PORT(PORT_CE) &= ~_BV(PIN_CE) // Макрос активации чипов дисплея                       | 0 на CE
#define hcms_disable()		PORT(PORT_CE) |= _BV(PIN_CE)  // Макрос деактивации чипов дисплея                     | 1 на CE
#if !defined(USE_SPI)    // Если НЕ используется аппаратный SPI
#define hcms_clk_hi()		PORT(PORT_CLK) |= _BV(PIN_CLK)  // Макрос установки высокого уровня в линии CLK  | 1 на CLK
#define hcms_clk_lo()		PORT(PORT_CLK) &= ~_BV(PIN_CLK) // Макрос установки низкого уровня в линии CLK   | 0 на CLK
#define hcms_di_hi()		PORT(PORT_DI) |= _BV(PIN_DI)  // Макрос установки высокого уровня в линии данных | 1 на DI
#define hcms_di_lo()		PORT(PORT_DI) &= ~_BV(PIN_DI) // Макрос установки низкого уровня в линии данных  | 0 на DI
#else
#define hcms_clk_hi() // Макрос установки высокого уровня в линии CLK
#define hcms_clk_lo() // Макрос установки низкого уровня в линии CLK
#define hcms_di_hi() // Макрос установки высокого уровня в линии данных
#define hcms_di_lo() // Макрос установки низкого уровня в линии данных
#endif

/** Сброс дисплея
 * Выполняется единственный раз - при инициализации
 */
static void hcms_reset(void){
	PORT(PORT_RST) &= ~_BV(PIN_RST); // Установка низкого уровня на RST
	PORT(PORT_RST) |= _BV(PIN_RST);  // Установка высокого уровня на RST
}

/** Вывод байта столбца символа в дисплей
 * Реализует последовательный вывод байта в зависимости от значения #USE_SPI
 * либо программно, либо аппаратно
 * Аппаратный вывод примерно в 5 раз быстрее программного
 * @param c
 */
#if !defined(USE_SPI) // Если не используется аппаратный SPI
static void hcms_put_byte(uint8_t c){
	for(uint8_t mask = 0x80; mask; mask >>= 1){
		hcms_clk_lo(); // Установка низкого уровня в линии CLK | 0 на CLK
		if(c & mask)	hcms_di_hi(); // Установка высокого уровня в линии данных | 1 на DI
		else			hcms_di_lo(); // Установка низкого уровня в линии данных | 0 на DI
		hcms_clk_hi(); // Установка высокого уровня в линии CLK | 1 на CLK
	}
}
#else
static void hcms_put_byte(uint8_t c){
	SPDR = c;
	while(!(SPSR & _BV(SPIF)));
}
#endif

/** Обновление регистра управления
 * @param data - состояние регистра управления в виде одного байта
 */
static void hcms_ctrl_register(uint8_t data){
	hcms_ctrl_mode(); // Включение режима записи в регистры управления | 1 на RS
	hcms_enable(); // Активации чипов дисплея | 0 на CE
	hcms_put_byte(data);
	hcms_clk_lo(); // Установка низкого уровня в линии CLK | 0 на CLK
	hcms_disable(); // Деактивация чипов дисплея | 1 на CE
}

// Вывод "сырых" байтов
void hcms_raw_pixels(uint8_t *buf, buf_size_t sz){
	hcms_data_mode();
	hcms_enable();
	for(buf_size_t i=0; i < sz; i++){
		hcms_put_byte(buf[i]);
	}
	hcms_clk_lo();
	hcms_disable();
}

static void rpad_strcpy(char *d, char *s){ // Копирует строку *s в строку *d
	uint8_t l = strlen(s); // Записываем количество символов в строке в переменную "l"
	for(uint8_t i=0; i<SCR_SIZE; i++)
		if(i >= l)
			d[i] = ' '; // Если символы в строке закончились, забиваем остаток буфера пустышками
		else
			d[i] = s[i]; // иначе записываем код символа в буфер
}

// Вывод строки
void hcms_puts(char *s){
	rpad_strcpy(hcms_screen, s); // Запихиваем в экранный символьный буфер коды символов строки
	hcms_update_scr();
}

// Вывод строки из flash
void hcms_puts_P(const char *s){
	char tmp[SCR_SIZE];
	strncpy_P(tmp, s, SCR_SIZE);
	hcms_puts(tmp);
}

/** Вычисление смещения начала битмапа символа в шрифте
 * Поддерживает кириллицу в кодировке Win-1251
 * @param c - символ
 * @return Смещение начала битмапа в массиве шрифта
 */
static const __flash uint8_t* get_offset_char(char c){
	const __flash uint8_t *ptr;
	char fc;

	fc = FONT_FIRST_CHAR;
	ptr = font5x7;

	switch(c){
	case FONT_FIRST_CHAR ... FONT_LAST_CHAR:
		break;
#if defined(SUPPORT_CYR)
	case 'А' ... 'я':
		fc = 'А';
		ptr = font_cyr;
		break;
	case 'Ё':
		fc = c;
		ptr = font_YO;
		break;
	case 'ё':
		fc = c;
		ptr = font_yo;
		break;
#endif
	case '\0':
		c = ' ';
		break;
	default:
		c = FONT_UNKNOWN_CHAR;
	}

	return ptr + (c - fc)*FONT_COLS;
}

#if defined(USE_CURSOR)
static uint8_t inverce;
#endif

static void hcms_put_char(char c){ // Отправить символ на дисплей
	const __flash uint8_t * font; // Смещение начала символа в шрифте
	font  = get_offset_char(c); // Вычисляем смещение
	// Вывод "столбиков" символа
	for(uint8_t i = 0; i < FONT_COLS; i++){ // Для каждого "столбика" символа
		// Для загрузки байта из flash используется макрос совместимости #p_get
		hcms_put_byte(p_get(font++)
#if defined(USE_CURSOR)
			// Если требуется курсор, то в нужной позиции инверсия символа
			^ (inverce ? 0xFF : 0)
#endif
		);
	}
}

// Очистка дисплея
void hcms_clrscr(void){
#if defined(USE_CURSOR)
	cursor_pos = NO_CURSOR;
#endif
	hcms_puts_P(PSTR(""));
}

// Обновление дисплея содержимым экранной области (буфера)
void hcms_update_scr(void){
	hcms_data_mode(); // Включение режима записи данных	| 0 на RS
	hcms_enable(); // Активации чипов дисплея | 0 на CE

	for(uint8_t pos = 0; pos < SCR_SIZE; pos++){ // Для каждого символа на дисплее
#if defined(USE_CURSOR)
		// Если требуется курсор, то в нужной позиции инверсия символа
		inverce = (pos == cursor_pos);
#endif
		hcms_put_char(hcms_screen[pos]); // Отправить символ на дисплей
	}
	hcms_clk_lo(); // Установка низкого уровня в линии CLK | 0 на CLK
	hcms_disable(); // Деактивация чипов дисплея | 1 на CE
}

#if defined(USE_CURSOR)
// Установка позиции курсора
void hcms_cursor_pos(uint8_t pos){
	cursor_pos = pos;
	hcms_update_scr();
}
#endif

// Управление яркостью
void hcms_bright(pwm_brightness_t br){
	def_ctrl_r0.brightness = br;
	hcms_ctrl_register(def_ctrl_r0.byte);
}

// Включение-отключение дисплея
void hcms_on(uint8_t on){
	def_ctrl_r0.sleep_mode = !on;
	hcms_ctrl_register(def_ctrl_r0.byte);
}

// Управление пиковым током
void hcms_peak_current(peak_current_t pc){
	def_ctrl_r0.peak_current = pc;
	hcms_ctrl_register(def_ctrl_r0.byte);
}

// Инициализация модуля
INIT(7){
#if defined(PORT_HCMS)
	DDR(PORT_HCMS) |= _BV(PIN_CE) | _BV(PIN_RS) | _BV(PIN_RST); // Линии порта - выходы
	PORT(PORT_HCMS) |= _BV(PIN_CE) | _BV(PIN_RS) | _BV(PIN_RST); // Установим их в 1
#else
	DDR(PORT_CE) |= _BV(PIN_CE);
	PORT(PORT_CE) |= _BV(PIN_CE);
	DDR(PORT_RS) |= _BV(PIN_RS);
	PORT(PORT_RS) |= _BV(PIN_RS);
	DDR(PORT_RST) |= _BV(PIN_RST);
#endif
	hcms_reset();

#if defined(USE_SPI)
	// Инициализация аппаратного SPI
	SPCR = _BV(SPE) | _BV(MSTR) | SPI_CLK_DIV_4;
	SPSR = _BV(SPI2X);
#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega32A__) || defined(__AVR_ATmega16__)
	DDRB |= _BV(PB7) | _BV(PB5) | _BV(PB4); // SS, MOSI, SCK
	DDRB &= ~ _BV(PB6); // MISO
#elif defined(__AVR_ATmega8__)
	DDRB |= _BV(PB2) | _BV(PB5) | _BV(PB3); // SS, MOSI, SCK
	DDRB &= ~_BV(PB4); // MISO
#else
#	error CONFIGURE MANUALLY SS PIN AS OUTPUT, and so MOSI and CLK.
#endif
#else
#if defined(PORT_HCMS)
	DDR(PORT_HCMS) |= _BV(PIN_CLK) | _BV(PIN_DI);
#else
	DDR(PORT_CLK) |= _BV(PIN_CLK);
	DDR(PORT_DI) |= _BV(PIN_DI);
#endif
	PORT(PORT_CLK) &= ~_BV(PIN_CLK);
#endif

	// Задаем состояние по умолчанию регистров управления
	def_ctrl_r0.brightness = DEFAULT_BRIGHTNESS;
	def_ctrl_r0.peak_current = DEFAULT_PEAK_CURRENT;
	def_ctrl_r0.sleep_mode = 1;
	def_ctrl_r0.reg = 0;

	def_ctrl_r1.data_out_ctrl = 1;
	def_ctrl_r1.edo_prescaler = 0;
	def_ctrl_r1.reserved = 0;
	def_ctrl_r1.reg = 1;

	hcms_clrscr();
	// В регистр управления 1 надо записать бит параллельной
	// загрузки регистров управления, чтобы в последующем обновлять эти
	// регистры одновременно в одном цикле записи байта. Но при инициализации
	// придется повторить запись управляющего слова в 1-й регистр столько раз,
	// сколько всего отдельных чипов.
	for(uint8_t i=0; i < CHIP_CNT+1; i++)
		hcms_ctrl_register(def_ctrl_r1.byte);
	hcms_ctrl_register(def_ctrl_r0.byte);
}

/*****************************************************************************
 * Далее следуют "презентационные" функции, практическая ценность которых
 * сомнительна, но зато они позволяют сделать красивую демонстрацию.
 *****************************************************************************/

/** Смена одного символа другим сверху вниз
 * Функция сдвигает на дисплее один символ вниз, дополняя сверху знакоместо
 * строками второго символа
 * @param c0 - заменяемый символ (старый)
 * @param c1 - заменяющий символ (новый)
 * @param shift - номер строки в символе (1..8)
 */
static void shiftdn_two_char(uint8_t c0, uint8_t c1, uint8_t shift){
	uint8_t b0, b1;
	for(uint8_t i=0; i < FONT_COLS; i++){ // FONT_COLS - количество столбцов в знакоместе (5)
		b0 = p_get(get_offset_char(c0) + i); // код столбца старого символа
		b1 = p_get(get_offset_char(c1) + i); // код столбца нового символа
		for(uint8_t sh = shift; sh; sh--){ // Для каждой строки одного символа
			b0 = (b0 << 1) | (b1 & 0x80 ? 1 : 0); // если b1 & 0x80 - истина, то |1 иначе |0
			b1 <<= 1; // переходим к следующему биту кода столбца
		}
		hcms_put_byte(b0);
	}
}

/** Смена одного символа другим снизу вверх
 * Функция сдвигает на дисплее один символ вверх, дополняя снизу знакоместо
 * строками второго символа
 * @param c0 - заменяемый символ (старый)
 * @param c1 - заменяющий символ (новый)
 * @param shift - номер строки в символе (1..8)
 */
static void shiftup_two_char(uint8_t c0, uint8_t c1, uint8_t shift){
	uint8_t b0, b1;
	for(uint8_t i=0; i < FONT_COLS; i++){
		b0 = p_get(get_offset_char(c0) + i);
		b1 = p_get(get_offset_char(c1) + i);
		for(uint8_t sh = shift; sh; sh--){
			b0 = (b0 >> 1) | (b1 & 1 ? 0x80 : 0);
			b1 >>= 1;
		}
		hcms_put_byte(b0);
	}
}

/** Проверка массива байтов на "все нули"
 * Вспомогательна функция
 * @param buf - проверяемый массив байтов
 * @param sz - количество проверяемых байтов
 * @return 0, если есть хотя бы один ненулевой байт, 1 - если все байты в
 * массиве равны нулю
 */
static uint8_t is_clear(uint8_t *buf, uint8_t sz){
	uint8_t result = 0;
	for(;sz;sz--)
		result |= *buf++;
	return result == 0;
}

static char ns[SCR_SIZE]; // Вспомогательный буфер для реализации эффектов

/** Замена символа из экранной области на новый из вспомогательного буфера (только для измениющихся символов)
 * Реализует эффект замены всплытие или опускание
 * снизу (всплытие) или сверху (опускание)
 * @param up - направление: 1 - вверх, -1 - вниз
 * @param pos - обрабатываемый символ
 * @param shift - номер строки в символе (1..8)
 */
static void hcms_rollover_chars(int8_t up, uint8_t pos, uint8_t shift){
	if(hcms_screen[pos] != ns[pos]){ // Если символ экранной области не совпадает с символом новой строки
		if(up < 0)	shiftdn_two_char(hcms_screen[pos], ns[pos], shift);
		else		shiftup_two_char(hcms_screen[pos], ns[pos], shift);
	} else {
		hcms_put_char(ns[pos]);
	}
}

/** Замена символа из экранной области на новый из вспомогательного буфера (для ВСЕХ символов строки)
 * Реализует эффект замены всплытие или опускание
 * снизу (всплытие) или сверху (опускание)
 * @param up - направление: 1 - вверх, -1 - вниз
 * @param pos - обрабатываемый символ
 * @param shift - номер строки в символе (1..8)
 */
static void hcms_rollover_chars_all(int8_t up, uint8_t pos, uint8_t shift){
	if(up < 0)	shiftdn_two_char(hcms_screen[pos], ns[pos], shift);
	else		shiftup_two_char(hcms_screen[pos], ns[pos], shift);
}

// Вывод строки с эффектом всплывания/опускания (только для измениющихся символов)
// Совпадающие символы не меняются, не совпадающие - замещаются новыми
void hcms_rollower_puts(char *s, int8_t up){
	rpad_strcpy(ns, s); // Копируем в промежуточный буфер строку (16 символов! Пустышки - пробелы)
	for(uint8_t sh=1; sh <=8; sh++){ // Цикл по количеству строк в символе
		hcms_data_mode(); // Включение режима записи данных	| 0 на RS
		hcms_enable(); // Активации чипов дисплея | 0 на CE
		// Обработка каждого знакоместа
		for(uint8_t pos=0; pos < SCR_SIZE; pos++){
			hcms_rollover_chars(up, pos, sh); // Выполняем замену текущего символа на дисплее новым из буфера
		}
		hcms_clk_lo(); // Установка низкого уровня в линии CLK | 0 на CLK
		hcms_disable(); // Деактивация чипов дисплея | 1 на CE
		_delay_ms(ANIMATE_DELAY_MS); // Задержка для зрелищности
	}
	memcpy(hcms_screen, ns, SCR_SIZE); // Перенос новой строки в экранную область
}

// Вывод строки с эффектом всплывания/опускания (для ВСЕХ символов строки)
// Все символы строки замещаются новыми
void hcms_rollower_puts_all(char *s, int8_t up){
	rpad_strcpy(ns, s); // Копируем в промежуточный буфер строку (16 символов! Пустышки - пробелы)
	for(uint8_t sh=1; sh <=8; sh++){ // Цикл по количеству строк в символе
		hcms_data_mode(); // Включение режима записи данных	| 0 на RS
		hcms_enable(); // Активации чипов дисплея | 0 на CE
		// Обработка каждого знакоместа
		for(uint8_t pos=0; pos < SCR_SIZE; pos++){
			hcms_rollover_chars_all(up, pos, sh); // Выполняем замену текущего символа на дисплее новым из буфера
		}
		hcms_clk_lo(); // Установка низкого уровня в линии CLK | 0 на CLK
		hcms_disable(); // Деактивация чипов дисплея | 1 на CE
		_delay_ms(ANIMATE_DELAY_MS); // Задержка для зрелищности
	}
	memcpy(hcms_screen, ns, SCR_SIZE); // Перенос новой строки в экранную область
}

// Вывод строки с эффектом сдвига влево
// Все символы строки замещаются новыми
void hcms_shiftleft_puts(char *s){
	rpad_strcpy(ns, s); // Копируем в промежуточный буфер строку (16 символов! Пустышки - пробелы)
	for(int8_t pos=-5; pos <=SCR_SIZE; pos++){ // Для всех символов строки
		for(uint8_t i=SCR_SIZE-1; i>0; i--){
			hcms_screen[i] = hcms_screen[i-1]; // Сдвигаем вправо все символы кроме первого
		}
		if (pos<1)
			hcms_screen[0] = ' '; // Записываем пробел в первую ячейку строки для разделения строк
		else
			hcms_screen[0] = ns[SCR_SIZE-pos]; // Записываем новый символ в первую ячейку строки
		hcms_puts(hcms_screen); // Вывод строки на дисплей
		_delay_ms(ANIMATE_DELAY_MS); // Задержка для зрелищности
	}
	memcpy(hcms_screen, ns, SCR_SIZE); // Перенос новой строки в экранную область
}

// Вывод строки с эффектом сдвига вправо
// Все символы строки замещаются новыми
void hcms_shiftright_puts(char *s){
	rpad_strcpy(ns, s); // Копируем в промежуточный буфер строку (16 символов! Пустышки - пробелы)
	for(int8_t pos=-6; pos <SCR_SIZE; pos++){ // Для всех символов строки
		for(uint8_t i=0; i <SCR_SIZE-1; i++){
			hcms_screen[i] = hcms_screen[i+1]; // Сдвигаем влево все символы кроме последнего
		}
		if (pos<0)
		hcms_screen[SCR_SIZE-1] = ' '; // Записываем пробел в последнюю ячейку строки для разделения строк
		else
		hcms_screen[SCR_SIZE-1] = ns[pos]; // Записываем новый символ в последнюю ячейку строки
		hcms_puts(hcms_screen); // Вывод строки на дисплей
		_delay_ms(ANIMATE_DELAY_MS); // Задержка для зрелищности
	}
	memcpy(hcms_screen, ns, SCR_SIZE); // Перенос новой строки в экранную область
}

// Вывод строки с эффектом последовательного всплывания/опускания (только для измениющихся символов)
// Совпадающие символы не меняются, не совпадающие - замещаются новыми (каждый следующий символ запаздывает после предыдущего)
void hcms_smooth_rollower_puts(char *s, int8_t up){
	rpad_strcpy(ns, s); // Копируем в промежуточный буфер строку (16 символов! Пустышки - пробелы)

	static uint8_t sh[SCR_SIZE];	// Массив шагов сдвига
	memset(sh, 0, SCR_SIZE);		// Изначально он обнулен,
	sh[0] = 1;						// кроме самого первого элемента

	// Цикл обновления длится, пока в массиве сдвигов есть хотя бы один
	// не завершенный шаг
	while(!is_clear(sh, SCR_SIZE)){
		hcms_data_mode();
		hcms_enable();

		// Каждую позицию на дисплее заменяем при помощи сдвига новым символом
		for(uint8_t pos=0; pos < SCR_SIZE; pos++){
			// Сдвиг у каждой позиции имеет свою величину - массив сдвигов
			hcms_rollover_chars(up, pos, sh[pos]);
		}
		// Обновляем массив сдвигов, обеспечивая плавное нарастание сдвига
		// Смещаем содержимое массива на 1 позицию
		memmove(sh+1, sh, SCR_SIZE-1);
		if(sh[0]){ // Если первый элемент не равен нулю
			if(++(sh[0]) > 8){ // Увеличиваем его на 1, но не больше, чем до 8
				// Если оказалось, что получился сдвиг на 9 - обнуляем,
				// т.к. сдвигать больше не надо
				sh[0] = 0;
			}
		}

		// Обновляем экранную область
		for(uint8_t i=0; i < SCR_SIZE; i++){
			// Перенося в нее все символы новой строки, для которых сдвиг уже завершен
			if(sh[i] == 0) hcms_screen[i] = ns[i];
			else break;
		}
		hcms_clk_lo();
		hcms_disable();
		_delay_ms(ANIMATE_DELAY_MS); // Задержка для зрелищности
	}
}

// Вывод строки с эффектом последовательного всплывания/опускания (для ВСЕХ символов строки)
// Совпадающие символы не меняются, не совпадающие - замещаются новыми (каждый следующий символ запаздывает после предыдущего)
void hcms_smooth_rollower_puts_all(char *s, int8_t up){
	rpad_strcpy(ns, s); // Копируем в промежуточный буфер строку

	static uint8_t sh[SCR_SIZE];	// Массив шагов сдвига
	memset(sh, 0, SCR_SIZE);		// Изначально он обнулен,
	sh[0] = 1;						// кроме самого первого элемента

	// Цикл обновления длится, пока в массиве сдвигов есть хотя бы один
	// не завершенный шаг
	while(!is_clear(sh, SCR_SIZE)){
		hcms_data_mode();
		hcms_enable();

		// Каждую позицию на дисплее заменяем при помощи сдвига новым символом
		for(uint8_t pos=0; pos < SCR_SIZE; pos++){
			// Сдвиг у каждой позиции имеет свою величину - массив сдвигов
			hcms_rollover_chars_all(up, pos, sh[pos]);
		}
		// Обновляем массив сдвигов, обеспечивая плавное нарастание сдвига
		// Смещаем содержимое массива на 1 позицию
		memmove(sh+1, sh, SCR_SIZE-1);
		if(sh[0]){ // Если первый элемент не равен нулю
			if(++(sh[0]) > 8){ // Увеличиваем его на 1, но не больше, чем до 8
				// Если оказалось, что получился сдвиг на 9 - обнуляем,
				// т.к. сдвигать больше не надо
				sh[0] = 0;
			}
		}

		// Обновляем экранную область
		for(uint8_t i=0; i < SCR_SIZE; i++){
			// Перенося в нее все символы новой строки, для которых сдвиг уже завершен
			if(sh[i] == 0) hcms_screen[i] = ns[i];
			else break;
		}
		hcms_clk_lo();
		hcms_disable();
		_delay_ms(ANIMATE_DELAY_MS); // Задержка для зрелищности
	}
}