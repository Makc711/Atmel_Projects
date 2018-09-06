#include "keyboardXY.h"
//-----------------------------------------
volatile uint8_t pressedKey = key_null; // Настоящее значение кода нажатой кнопки
uint8_t comp=0; // Используется для защиты от дребезга
uint8_t but_press = FALSE; // Флаг для многократного "нажатия" на кнопку при ее удержании
uint8_t time_press = 5; // Время между "нажатиями" на кнопку при ее удержании
uint8_t time_press_long = TIMEPRESS_SLOW; // Время между "нажатиями" на кнопку при ее удержании
uint8_t click = 0; // Счетчик "медленных нажатий" на кнопку
//-----------------------------------------
uint16_t tim_cnt=0;// Счетчик тиков таймера (нужен для мигания пунктами меню)
extern uint8_t seccnt; // Счетчик - нужен для мигания ":" в часах
extern uint8_t secstatus; // Состояние ":" в часах (FALSE - погашен/TRUE - горит)
// Переменные для задержек DS18B20
extern uint16_t tim_cntDS; // Счетчик тиков для задержек DS18B20
extern uint8_t startDS; // Флаг начала преобразования
extern uint8_t stopDS; // Флаг завершения преобразования
// Переменные для задержек twi
extern uint8_t writeTime; // Таймер (нужен для отсчета времени между циклами записи eepromext)

extern uint8_t flag_set_led; // Флаг для индикации 7-сег индикатора (TRUE - установленное значение; FALSE - измеренное значение)
uint16_t set_led_cnt = 0; // Счетчик для выдержки времени для индикации установленного значения на 7-сег
//-----------------------------------------

void init_button_timer(void){
	TCCR0 |= (1<<CS02)|(0<<CS01)|(0<<CS00); // Устанавливаем предделитель 64
	// тем самым получаем - частота тактирования (7372800) / предделитель / 256 = 450 (около 2-х милисекунд)
	// делим на 256, так как таймер нулевой всегда считает от 0x00 до 0xFF и это не регулируется никак
	TIMSK |= (1<<TOIE0); // Устанавливаем бит разрешения прерывания 0-ого счетчика по переполнению
}
//----------------------------------------
INIT(7){ // Инициализация клавиатуры
#if defined(PORT_KEYBOARD)
	DDR(PORT_KEYBOARD) &= ~(_BV(PIN_stolb1) | _BV(PIN_stolb2) | _BV(PIN_stolb3) | _BV(PIN_stroka1) | _BV(PIN_stroka2)); // Линии порта - входы
	PORT(PORT_KEYBOARD) |= _BV(PIN_stolb1) | _BV(PIN_stolb2) | _BV(PIN_stolb3) | _BV(PIN_stroka1) | _BV(PIN_stroka2); // с подтягивающими резисторами
#else
 	DDR(PORT_stolb1) &= ~_BV(PIN_stolb1); // Столбцы - входы
 	PORT(PORT_stolb1) |= _BV(PIN_stolb1); // Столбцы - входы с подтягивающими резисторами
 	DDR(PORT_stolb2) &= ~_BV(PIN_stolb2); // Столбцы - входы
 	PORT(PORT_stolb2) |= _BV(PIN_stolb2); // Столбцы - входы с подтягивающими резисторами
 	DDR(PORT_stolb3) &= ~_BV(PIN_stolb3); // Столбцы - входы
 	PORT(PORT_stolb3) |= _BV(PIN_stolb3); // Столбцы - входы с подтягивающими резисторами
	DDR(PORT_stroka1) &= ~_BV(PIN_stroka1); // Строки - тоже входы
	PORT(PORT_stroka1) |= _BV(PIN_stroka1); // Строки - тоже входы с подтягивающими резисторами
	DDR(PORT_stroka2) &= ~_BV(PIN_stroka2); // Строки - тоже входы
	PORT(PORT_stroka2) |= _BV(PIN_stroka2); // Строки - тоже входы с подтягивающими резисторами
#endif
// Примечание: _BV(3) аналогично команде (1<<3) (поразрядный сдвиг)
// Например: PORTD|=_BV(3); == PORTD|=(1<<3); => 0bxxxx1xxx // Включили внутренний подтягивающий резистор к линии 3 порта D
//			 DDRD&=~_BV(3); == DDRD&=~(1<<3); // Установка линии 3 порта D на вход
	
	init_button_timer(); // Инициализация таймера
}

 void KEYB_ScanKeyboard (void){
	uint8_t key = 0;
	
// Используется программная защита от КЗ	
 	DDR(PORT_stroka1) |= _BV(PIN_stroka1); // Делаем строку 1 выходом
 	PORT(PORT_stroka1) &= ~_BV(PIN_stroka1); _delay_us(100); // и даём в неё 0
		 if (BitIsClear(PIN(PORT_stolb1), PIN_stolb1)) key = key11; // Если он в 1-м столбце, то нажата клавиша 11
	else if (BitIsClear(PIN(PORT_stolb2), PIN_stolb2)) key = key12; // Если на линии PORT_stolb2 порта PIN(PORT_stolb2) присутствует "0" 
	else if (BitIsClear(PIN(PORT_stolb3), PIN_stolb3)) key = key13;
	PORT(PORT_stroka1) |= _BV(PIN_stroka1); // В строку 1 возвращаем 1
	DDR(PORT_stroka1) &= ~_BV(PIN_stroka1); // И делаем её входом
	
	DDR(PORT_stroka2) |= _BV(PIN_stroka2);
	PORT(PORT_stroka2) &= ~_BV(PIN_stroka2); _delay_us(100);
		 if (BitIsClear(PIN(PORT_stolb1), PIN_stolb1)) key = key21;
	else if (BitIsClear(PIN(PORT_stolb2), PIN_stolb2)) key = key22;
	else if (BitIsClear(PIN(PORT_stolb3), PIN_stolb3)) key = key23;
	PORT(PORT_stroka2) |= _BV(PIN_stroka2);
	DDR(PORT_stroka2) &= ~_BV(PIN_stroka2);
		
	DDR(PORT_stroka3) |= _BV(PIN_stroka3);
	PORT(PORT_stroka3) &= ~_BV(PIN_stroka3); _delay_us(100);
		if (BitIsClear(PIN(PORT_stolb1), PIN_stolb1)) key = key31;
	else if (BitIsClear(PIN(PORT_stolb2), PIN_stolb2)) key = key32;
	else if (BitIsClear(PIN(PORT_stolb3), PIN_stolb3)) key = key33;
	PORT(PORT_stroka3) |= _BV(PIN_stroka3);
	DDR(PORT_stroka3) &= ~_BV(PIN_stroka3);	

	  if (key){ // Если во временной переменной "key" что-то есть
		  if (but_press){ // Если флаг для многократного "нажатия" поднят
			  if (click < CLICKS) time_press = TIMEPRESS_SLOW; // "Медленные нажатия" на кнопку
			  else time_press = TIMEPRESS_FAST; // "Быстрые нажатия" на кнопку
			  time_press_long = time_press;
		  }
		  else time_press = 5;
		  if (comp == THRESHOLD){ // и если кнопка удерживается THRESHOLD циклов опроса
			  comp = THRESHOLD+10; // Чтобы при удержании кнопки pressedKey не обновлялась
			  pressedKey = key; // Записать ее номер в буфер
			  return;
		  }
		  else if (comp < (THRESHOLD + time_press)) // Если кнопка удерживается меньше (THRESHOLD+time_press) циклов опроса (защита от дребезга)
			  comp++; // (THRESHOLD+200) циклов опроса кнопки - примерно 0,5 сек
		  else if (comp >= (THRESHOLD + time_press_long)){ // Если кнопка удерживается больше (THRESHOLD+TIMEPRESS) циклов опроса
			  comp=0; // Обнуляем, чтобы была возможность обновить pressedKey
			  if (click < 255) click++; // Считаем количество "медленных нажатий" на кнопку
		  }
	  }
	  else {
		  but_press = FALSE; // Запрещаем многократное "нажатие" на кнопку
		  comp=0; // Обнуляем при отпускании кнопки
		  click = 0; // Обнуляем счетчик "медленных нажатий" на кнопку
	  }
}

// Возвращает код нажатой кнопки
uint8_t KEYB_GetKey(void){
	uint8_t key = pressedKey;
	pressedKey = key_null;
	return key;
}

ISR (TIMER0_OVF_vect){
//===============КЛАВИАТУРА================
	KEYB_ScanKeyboard(); // Сканируем клавиатуру
//=============МИГАЛКА МЕНЮ================
	if (tim_cnt++ == TICKSEC) // Если досчитали до 1 сек
		tim_cnt = 0; // Счетчик нужен для мигания пунктами меню
//=======ЗАДЕРЖКА 1 сек для 7-сег==========
	if (flag_set_led){ // Если флаг поднялся (изменяется значение)
		if (set_led_cnt++ == TICKSEC){ // Если досчитали до 1 сек
			set_led_cnt = 0; // Сбрасываем счетчик
			flag_set_led = FALSE; // Снимаем флаг (выводим измеренное значение)
		}
	}
//=====СЧЕТЧИК ДЛЯ ЗАДЕРЖЕК DS18B20========
	if (startDS) // Если преобразование было начато
		tim_cntDS++; // Считаем тики
	if (tim_cntDS == TICK750){ // Если прошло 750 мс
		startDS = FALSE; // Снимаем флаг (останавливаем счетчик, можно завершать преобразование)
		tim_cntDS = 0; // Сбрасываем счетчик
		stopDS = TRUE; // Поднимаем флаг (можно завершать преобразование)
	}
//=========МИГАЛКА ":" В ЧАСАХ=============
	if (seccnt < 255) 
		seccnt++; // Счетчик нужен для мигания ":" в часах (может досчитать до 0,566 сек.)
	if (seccnt == HALFSEC) 
		secstatus = TRUE; // Зажигаем ":" в часах
//=====СЧЕТЧИК ДЛЯ ЗАДЕРЖЕК twi========
	if (writeTime>0)
		writeTime--; // Ожидание между циклами записи в eepromext
}
//-----------------------------------------