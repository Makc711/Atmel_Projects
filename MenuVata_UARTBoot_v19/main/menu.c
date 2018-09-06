#include "menu.h"
//-----------------------------------------------------------
//--------------ОБЪЯВЛЕНИЕ ПРОТОТИПОВ ФУНЦИЙ-----------------
void MainMenuProcess(void); // Функция главного меню
void SubmenuClockProcess(void); // Функция подменю "Часы"
void SubmenuMemoryProcess(void); // Функция подменю "Память:"
void SubmenuSettingsProcess(void); // Функция подменю "Настройки"
//-----------------------------------------------------------
char str_disp[16]; // Массив для вывода строки на дисплей
char str_prog16[16]; // Буффер для хранения строк из flash размером 16 байт
char str_prog3[3]; // Буффер для хранения строк из flash размером 3 байта
//-----------------------------------------------------------
uint8_t button_state = key_null; // Код нажатой кнопки
extern uint8_t but_press; // Флаг для многократного "нажатия" на кнопку при ее удержании (0-нет многократного нажатия, иначе - есть)
uint8_t direction = DIRECTION_NO; // Направление движения по меню
//===============ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ===============
extern uint16_t tim_cnt; // Счетчик тиков таймера (нужен для мигания выбранного пункта меню)
extern uint8_t mday, // День месяца [1;31]
			   month, // Месяц [1;12]
			   year, // Год [0;99]
			   wday, // День недели [1;7]
			   hour, // Час [0;24]
			   minute, // Минута [0;59]
			   second; // Секунда [0;59]
extern uint16_t temper_set; // Установленная температура, 'С [15000;25000]/100
extern uint16_t rpm_set; // Установленная частота вращения, об/мин
extern uint8_t voltage; // Напряжение сети, В
extern uint8_t current_total; // Общий ток, *10А
extern uint16_t power_cons; // Расход мощности, кВт*ч
//------Переменные для тахометра------
extern uint8_t t2_1; // Количество текущих тиков таймера-счетчика 2
extern uint16_t t2_cycles; // Количество прерываний теймера-счетчика 0 c момента прыдыдущего внешнего прерывания по нарастающему фронту
extern uint32_t t2_ticks_int; // Количество тиков таймера-счетчика 0 за полный оборот
extern uint8_t flag_int; // Флаг для прерываний
extern uint16_t rpm; // Измеренная частота вращения, об/мин (мгновенное значение)
extern float rpmX; // Усредненное значение измеренной частоты вращения, об/мин
extern uint16_t get_rpm; // Измеренная частота вращения, об/мин
//-------Переменные для мотора--------
extern uint8_t pwm_mot; // Уровень ШИМ для МОТОРА [0;255]
//------------------------------------
extern uint8_t seccnt; // Счетчик - нужен для мигания ":" в часах
extern uint8_t secstatus; // Состояние ":" в часах (0 - погашен/1 - горит)
//----------ПЕРЕМЕННЫЕ ДЛЯ ПИД (ТЕМПЕРАТУРА)---------
extern uint8_t pwm_ten; // Уровень ШИМ [0;100] (ТЭН)
extern float integral; // Интегральная составляющая
extern float e1; // Первый такт ошибки (текущая ошибка)
extern uint8_t start; // Флаг запуска работы (0 - СТОП/1 - СТАРТ)
//-----------ПЕРЕМЕННЫЕ ДЛЯ ИСТОРИИ РАБОТЫ-----------
extern uint16_t freememory; // Свободная EEPROM память *10, %
//-------ПЕРЕМЕННЫЕ ДЛЯ ПУНКТА МЕНЮ "Настройки"------
extern uint8_t brightness_seg; // Яркость 7-сегментного индикатора, %
extern uint8_t brightness_led; // Яркость LED-индикатора, % (шаг изменения 2 %)
extern uint8_t reference_U; // Измеренное напряжение откалиброванным вольтметром, В
extern uint8_t reference_I; // Измеренный ток откалиброванным амперметром, *100А
extern uint16_t reference_UADC; // Измеренное значение U АЦП при reference_U, бит
extern uint16_t reference_IADC; // Измеренное значение I АЦП при reference_I, бит
extern uint16_t voltage_ADC; // Напряжение сети, бит [0;1023]
extern uint16_t current_ADC; // Общий ток, бит [0;1023]
//===================================================
//--------------ПУНКТЫ ГЛАВНОГО МЕНЮ-----------------
const char MMenuItem1[] PROGMEM = "Нагрев до: ";
const char MMenuItem2[] PROGMEM = "RPM: ";
//const char MMenuItem3[] PROGMEM = "RPM slow: ";
const char MMenuItem3[] PROGMEM = "Расход: ";
const char MMenuItem4[] PROGMEM = "Дата и время";
const char MMenuItem5[] PROGMEM = "Напряжение: ";
const char MMenuItem6[] PROGMEM = "Ток: ";
//const char MMenuItem8[] PROGMEM = "Темп-ра: ";
const char MMenuItem7[] PROGMEM = "Память: "; // (Memory) Копировать на SD; Очистить? да нет; 
const char MMenuItem8[] PROGMEM = "Настройки"; // (Settings) Яркость 7-сегм.; Яркость LED инд.; Калибровка U; Калибровка I; 
//------------ПУНКТЫ ПОДМЕНЮ "Память:"----------------
const char MemoryItem1[] PROGMEM = "Копировать на SD"; // Прогресс: 100%; Нет SD карты;
const char MemoryItem2[] PROGMEM = "Очистить память"; // Прогресс: 100%; Память очищена; Bad memory; 
const char MemoryItem3[] PROGMEM = "   Да     Нет   ";
const char MemoryItem4[] PROGMEM = "Прогресс: "; // (Прогресс: 100%)
const char MemoryItem5[] PROGMEM = "  Скопировано   ";
const char MemoryItem6[] PROGMEM = "  Нет SD карты  ";
const char MemoryItem7[] PROGMEM = " Память очищена ";
const char MemoryItem8[] PROGMEM = "   Bad memory   ";
//-----------ПУНКТЫ ПОДМЕНЮ "Настройки"---------------
const char SettingsItem1[] PROGMEM = "Яркость 7-сегм.";
const char SettingsItem2[] PROGMEM = "Яркость LED инд.";
const char SettingsItem3[] PROGMEM = "Калибровка U";
const char SettingsItem4[] PROGMEM = "Калибровка I";
//-------------НАИМЕНОВАНИЕ ДНЕЙ НЕДЕЛИ---------------
const char ItemDay1[] PROGMEM = "ПН";
const char ItemDay2[] PROGMEM = "ВТ";
const char ItemDay3[] PROGMEM = "СР";
const char ItemDay4[] PROGMEM = "ЧТ";
const char ItemDay5[] PROGMEM = "ПТ";
const char ItemDay6[] PROGMEM = "СБ";
const char ItemDay7[] PROGMEM = "ВС";
PGM_P const string_tableDay[] PROGMEM = {
	ItemDay1, 
	ItemDay2, 
	ItemDay3, 
	ItemDay4, 
	ItemDay5, 
	ItemDay6, 
	ItemDay7
};
//-------------НАИМЕНОВАНИЕ МЕСЯЦЕВ---------------
const char ItemMonth1[]  PROGMEM = "янв";
const char ItemMonth2[]  PROGMEM = "фев";
const char ItemMonth3[]  PROGMEM = "мар";
const char ItemMonth4[]  PROGMEM = "апр";
const char ItemMonth5[]  PROGMEM = "май";
const char ItemMonth6[]  PROGMEM = "июн";
const char ItemMonth7[]  PROGMEM = "июл";
const char ItemMonth8[]  PROGMEM = "авг";
const char ItemMonth9[]  PROGMEM = "сен";
const char ItemMonth10[] PROGMEM = "окт";
const char ItemMonth11[] PROGMEM = "ноя";
const char ItemMonth12[] PROGMEM = "дек";
PGM_P const string_tableMonth[] PROGMEM = {
	ItemMonth1, 
	ItemMonth2, 
	ItemMonth3, 
	ItemMonth4, 
	ItemMonth5, 
	ItemMonth6, 
	ItemMonth7,
	ItemMonth8,
	ItemMonth9,
	ItemMonth10,
	ItemMonth11,
	ItemMonth12
};
//----------------МИГАЛКА ":" В ЧАСАХ------------------
char secsta[2][2]= // 2 значения, 1 символ + 1 на \0 - конец строки
{
	" \0",
	":\0",
};
//-----------------------------------------------------
// Задаем начальный статус меню:
MENU_StateTypeDef menu_state = MENU_STATE_IDLE;
MAIN_MENU_StateTypeDef main_menu_state = MAIN_MENU_STATE_MM1_IDLE;
SUBMENU_CLOCK_StateTypeDef submenu_clock_state = SUBMENU_CLOCK_STATE_IDLE;
SUBMENU_MEMORY_StateTypeDef submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE;
SUBMENU_SETTINGS_StateTypeDef submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE;
//-----------------------------------------------------
void direction_str_disp(char *s){ // Выводим строку str_disp с направлением вывода на LED индикатор
	switch (direction)
	{
		case DIRECTION_DOWN: // Если переходим вниз
			direction = DIRECTION_NO; // Нет направления перехода
			hcms_rollower_puts_all(s, DOWN);
			break;
		case DIRECTION_UP: // Если переходим вверх
			direction = DIRECTION_NO; // Нет направления перехода
			hcms_rollower_puts_all(s, UP);
			break;
		case DIRECTION_LEFT: // Если переходим влево
			direction = DIRECTION_NO; // Нет направления перехода
			hcms_shiftleft_puts(s); 
			break;
		case DIRECTION_RIGHT: // Если переходим вправо
			direction = DIRECTION_NO; // Нет направления перехода
			hcms_shiftright_puts(s);
			break;
		default: // Если нет направления перехода
			hcms_puts(s); 
	}
}

void MenuProcess(void) // Функция меню
{
	switch (menu_state) // Смотрим на статус меню
	{
		case MENU_STATE_IDLE: // Старт программы
			strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
			sprintf(str_disp," %2u%s%02u %s %2u-%02u",hour,secsta[secstatus],minute,str_prog3,mday,month); // Запись строки в буфер
			direction_str_disp(str_disp); // Вывод строки на дисплей
			menu_state = MENU_STATE_WAIT; // Установить статус: "Ждем запуска главного меню"
			break;
		case MENU_STATE_WAIT: // Ждем запуска главного меню (нажатия кнопки)
			strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
			sprintf(str_disp," %2u%s%02u %s %2u-%02u",hour,secsta[secstatus],minute,str_prog3,mday,month); // Запись строки в буфер
			hcms_puts(str_disp); // Вывод строки на дисплей
			button_state = KEYB_GetKey(); // Записываем код нажатой кнопки в переменную
			if (button_state == Butt_Select){ // Если произошло нажатие кнопки Select
				direction = DIRECTION_RIGHT; // Переходим направо
				menu_state = MENU_STATE_MAIN; // Установить статус: "Запуск главного меню"
			}
			break;
		case MENU_STATE_MAIN: // Запуск главного меню
		//##### ОСТАНОВКА РАБОТЫ НАГРЕВАТЕЛЯ И МОТОРА #####
		//___Вывод строки на 7-сегментные индикаторы
			WriteStringMC14489("----", H_Alloff); // Отправить строку данных и точку
		//____Выключаем светодиоды___
			LED_TEN_OFF; // Выключаем светодиод ТЭН
			LED_START_OFF; // Выключаем светодиод ПУСК
		//_____Обнуляем тахометр_____
			TIMSK &= ~(1<<TOIE2); // Запрещаем прерывания при переполнении таймера-счетчика 2
			EIMSK &= ~(1<<INT4); // Запрещаем внешние прерывания INT4
			t2_1 = 0; // Обнуляем количество текущих тиков таймера-счетчика 2
			t2_cycles = 0; // Сбросим счетчик прерываний таймера-счетчика 2
			t2_ticks_int = 0; // Сбросим счетчик тиков таймера-счетчика 2 за полный оборот
			flag_int = TRUE; // Установим флаг для прерываний
			rpm = 0; // Сбросим измеренную частоту вращения
			rpmX = 0; // Сбросим усредненное значение частоты вращения
			get_rpm = 0; // Сбросим итоговое значение частоты вращения
		//_____Останавливаем мотор____
			pwm_mot = 0; // Обнуляем установленное значение ШИМ
			OCR1B = 0; // Сбрасываем ШИМ (МОТОР)
		//_______Выключаем ТЭН________
			pwm_ten = 0; // Устанавливаем ШИМ в 0
			PORT(PORT_PWMTEN) |= _BV(PIN_PWMTEN); // Подаем 1 (гасим ТЭН)
			integral = 0; // Обнуляем интеграл
			e1 = 0; // Обнуляем ошибку
		//____________________________
			start = 0; // Снимем флаг
		//#################################################
			main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // Установить статус меню: "Пункт 1 главного меню"
			MainMenuProcess(); // Вызов функции главного меню
			break;
	}
}
//-----------------------------------------------------
void MainMenuProcess(void) // Функция главного меню
{
uint16_t temper_set_tmp = temper_set; // Записываем текущее значение установленной температуры во временную переменную
uint16_t rpm_set_tmp = rpm_set; // Записываем текущее значение установленной частоты вращения во временную переменную
	while (TRUE)
	{
		//-----------------------------------------
		current_ADC = adc_data(ADC_CH_0); // Читаем значение АЦП (ток)
		voltage_ADC = adc_data(ADC_CH_2); // Читаем значение АЦП (напряжение)
		current_total = current_ADC * reference_I / reference_IADC; // Общий ток, *10А | *коэф (измеренный ток, А *10 / current_ADC) (1 А при 150 бит)
		voltage = (uint32_t) voltage_ADC * reference_U / reference_UADC; // Напряжение сети, В | *коэф (измеренное напряжение, В / voltage_ADC) (220 В при 900 бит)
		//-----------------------------------------
		switch (main_menu_state) // Смотрим на статус главного меню
		{
			case MAIN_MENU_STATE_MM1_IDLE: // Пункт 1 главного меню
				strcpy_P(str_prog16, MMenuItem1); // Копируем строку из flash в буффер ОЗУ
				sprintf(str_disp,"%s%3u`C",str_prog16,temper_set/100); // Запись строки в буфер
				direction_str_disp(str_disp); // Вывод пункта меню "Температура"
				main_menu_state = MAIN_MENU_STATE_MM1_WAIT; // Установить статус меню: "Ожидание выбора 1 пункта"
				break;
			case MAIN_MENU_STATE_MM2_IDLE: // Пункт 2 главного меню
				strcpy_P(str_prog16, MMenuItem2); // Копируем строку из flash в буффер ОЗУ
				sprintf(str_disp,"%s%4u об/мин",str_prog16,rpm_set); // Запись строки в буфер
				direction_str_disp(str_disp); // Вывод пункта меню "RPM"
				main_menu_state = MAIN_MENU_STATE_MM2_WAIT; // Установить статус меню: "Ожидание выбора 2 пункта"
				break;
			case MAIN_MENU_STATE_MM3_IDLE: // Пункт 3 главного меню
				strcpy_P(str_prog16, MMenuItem3); // Копируем строку из flash в буффер ОЗУ
				sprintf(str_disp,"%s%3uкВт*ч",str_prog16,power_cons); // Запись строки в буфер
				direction_str_disp(str_disp); // Вывод пункта меню "Расход"
				main_menu_state = MAIN_MENU_STATE_MM3_WAIT; // Установить статус меню: "Ожидание выбора 3 пункта"
				break;
			case MAIN_MENU_STATE_MM4_IDLE: // Пункт 4 главного меню
				strcpy_P(str_prog16, MMenuItem4); // Копируем строку из flash в буффер ОЗУ
				direction_str_disp(str_prog16); // Вывод пункта меню "Дата и время"
				main_menu_state = MAIN_MENU_STATE_MM4_WAIT; // Установить статус меню: "Ожидание выбора 4 пункта"
				break;
			case MAIN_MENU_STATE_MM5_IDLE: // Пункт 5 главного меню
				strcpy_P(str_prog16, MMenuItem5); // Копируем строку из flash в буффер ОЗУ
				sprintf(str_disp,"%s%3uВ",str_prog16,voltage); // Запись строки в буфер
				direction_str_disp(str_disp); // Вывод пункта меню "Напряжение"
				main_menu_state = MAIN_MENU_STATE_MM5_WAIT; // Установить статус меню: "Ожидание выбора 5 пункта"
				break;
			case MAIN_MENU_STATE_MM6_IDLE: // Пункт 6 главного меню
				strcpy_P(str_prog16, MMenuItem6); // Копируем строку из flash в буффер ОЗУ
				sprintf(str_disp,"%s%01u.%02uА",str_prog16,current_total/100,current_total%100); // Запись строки в буфер
				direction_str_disp(str_disp); // Вывод пункта меню "Ток"
				main_menu_state = MAIN_MENU_STATE_MM6_WAIT; // Установить статус меню: "Ожидание выбора 6 пункта"
				break;
			case MAIN_MENU_STATE_MM7_IDLE: // Пункт 7 главного меню
				strcpy_P(str_prog16, MMenuItem7); // Копируем строку из flash в буффер ОЗУ
				sprintf(str_disp,"%s%3u.%01u%%",str_prog16,freememory/10,freememory%10); // Запись строки в буфер
				direction_str_disp(str_disp); // Вывод пункта меню "Память:"
				main_menu_state = MAIN_MENU_STATE_MM7_WAIT; // Установить статус меню: "Ожидание выбора 7 пункта"
				break;
			case MAIN_MENU_STATE_MM8_IDLE: // Пункт 8 главного меню
				strcpy_P(str_prog16, MMenuItem8); // Копируем строку из flash в буффер ОЗУ
				direction_str_disp(str_prog16); // Вывод пункта меню "Настройка"
				main_menu_state = MAIN_MENU_STATE_MM8_WAIT; // Установить статус меню: "Ожидание выбора 8 пункта"
				break;
			case MAIN_MENU_STATE_MM1_WAIT: // Ожидание выбора 1 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						main_menu_state = MAIN_MENU_STATE_MM2_IDLE; // Установить статус меню: "Пункт 2 главного меню"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // Установить статус меню: "Пункт 8 главного меню"
						break;
					case Butt_Select:
						main_menu_state = MAIN_MENU_STATE_MM1; // Установить статус меню: "Выбор пункта 1"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						menu_state = MENU_STATE_IDLE; // Установить статус: "Старт программы"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM2_WAIT: // Ожидание выбора 2 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						main_menu_state = MAIN_MENU_STATE_MM3_IDLE; // Установить статус меню: "Пункт 3 главного меню"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // Установить статус меню: "Пункт 1 главного меню"
						break;
					case Butt_Select:
						main_menu_state = MAIN_MENU_STATE_MM2; // Установить статус меню: "Выбор пункта 2"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						menu_state = MENU_STATE_IDLE; // Установить статус: "Старт программы"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM3_WAIT: // Ожидание выбора 3 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						main_menu_state = MAIN_MENU_STATE_MM2_IDLE; // Установить статус меню: "Пункт 2 главного меню"
						break;
					case Butt_Select:
						main_menu_state = MAIN_MENU_STATE_MM3; // Установить статус меню: "Выбор пункта 3"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						menu_state = MENU_STATE_IDLE; // Установить статус: "Старт программы"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM4_WAIT: // Ожидание выбора 4 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						main_menu_state = MAIN_MENU_STATE_MM5_IDLE; // Установить статус меню: "Пункт 5 главного меню"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						main_menu_state = MAIN_MENU_STATE_MM3_IDLE; // Установить статус меню: "Пункт 3 главного меню"
						break;
					case Butt_Select:
						direction = DIRECTION_RIGHT; // Переходим направо
						submenu_clock_state = SUBMENU_CLOCK_STATE_IDLE; // Установить статус меню: "Подменю "Дата и время""
						SubmenuClockProcess(); // Вызов функции подменю "Часы"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						menu_state = MENU_STATE_IDLE; // Установить статус: "Старт программы"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM5_WAIT: // Ожидание выбора 5 пункта
				strcpy_P(str_prog16, MMenuItem5); // Копируем строку из flash в буффер ОЗУ
				sprintf(str_disp,"%s%3uВ",str_prog16,voltage); // Запись строки в буфер
				hcms_puts(str_disp); // Вывод пункта меню "Напряжение"
				//---------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						main_menu_state = MAIN_MENU_STATE_MM6_IDLE; // Установить статус меню: "Пункт 6 главного меню"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						menu_state = MENU_STATE_IDLE; // Установить статус: "Старт программы"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM6_WAIT: // Ожидание выбора 6 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						main_menu_state = MAIN_MENU_STATE_MM7_IDLE; // Установить статус меню: "Пункт 7 главного меню"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						main_menu_state = MAIN_MENU_STATE_MM5_IDLE; // Установить статус меню: "Пункт 5 главного меню"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						menu_state = MENU_STATE_IDLE; // Установить статус: "Старт программы"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM7_WAIT: // Ожидание выбора 7 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // Установить статус меню: "Пункт 8 главного меню"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						main_menu_state = MAIN_MENU_STATE_MM6_IDLE; // Установить статус меню: "Пункт 6 главного меню"
						break;
					case Butt_Select:
						direction = DIRECTION_RIGHT; // Переходим направо
						submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Память:""
						SubmenuMemoryProcess(); // Вызов функции подменю "Память:"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						menu_state = MENU_STATE_IDLE; // Установить статус: "Старт программы"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM8_WAIT: // Ожидание выбора 8 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // Установить статус меню: "Пункт 1 главного меню"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						main_menu_state = MAIN_MENU_STATE_MM7_IDLE; // Установить статус меню: "Пункт 7 главного меню"
						break;
					case Butt_Select:
						direction = DIRECTION_RIGHT; // Переходим направо
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Настройки""
						SubmenuSettingsProcess(); // Вызов функции подменю "Настройки"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						menu_state = MENU_STATE_IDLE; // Установить статус: "Старт программы"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM1: // Выбор пункта 1
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				strcpy_P(str_prog16, MMenuItem1); // Копируем строку из flash в буффер ОЗУ
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
					sprintf(str_disp,"%s%3u`C",str_prog16,temper_set_tmp/100); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"%s   `C",str_prog16); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if(temper_set_tmp > TEMPER_SETMIN) // Ограничение по минимальной температуре нагрева
							temper_set_tmp -= 100; // Уменьшаем температуру нагрева
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if(temper_set_tmp < TEMPER_SETMAX) // Ограничение по максимальной температуре нагрева
							temper_set_tmp += 100; // Увеличиваем температуру нагрева
						break;
					case Butt_Select:
						temper_set = temper_set_tmp; // Сохраняем измененное значение температуры нагрева
						EEPROM_write_word(ADDREEP_TEMPER_SET, temper_set_tmp); // Записать слово (2 байта) в EEPROM по адресу
						main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // Установить статус меню: "Пункт 1 главного меню"
						break;
					case Butt_Esc:
						temper_set_tmp = temper_set; // Возвращаем исходное заначение температуры нагрева во временную переменную
						main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // Установить статус меню: "Пункт 1 главного меню"
						break;
				}
				break;				
			case MAIN_MENU_STATE_MM2: // Выбор пункта 2
				strcpy_P(str_prog16, MMenuItem2); // Копируем строку из flash в буффер ОЗУ
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
					sprintf(str_disp,"%s%4u об/мин",str_prog16,rpm_set_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"%s     об/мин",str_prog16); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if(rpm_set_tmp > RPM_SETMIN) // Ограничение по минимальной RPM
							rpm_set_tmp -=100; // Уменьшаем частоту вращения
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if(rpm_set_tmp < RPM_SETMAX) // Ограничение по максимальной RPM
							rpm_set_tmp += 100; // Увеличиваем частоту вращения
						break;
					case Butt_Select:
						rpm_set = rpm_set_tmp; // Сохраняем измененное значение температуры нагрева
						EEPROM_write_word(ADDREEP_RPM_SET, rpm_set_tmp); // Записать слово (2 байта) в EEPROM по адресу
						main_menu_state = MAIN_MENU_STATE_MM2_IDLE; // Установить статус меню: "Пункт 2 главного меню"
						break;
					case Butt_Esc:
						rpm_set_tmp = rpm_set; // Возвращаем исходное заначение температуры нагрева во временную переменную
						main_menu_state = MAIN_MENU_STATE_MM2_IDLE; // Установить статус меню: "Пункт 2 главного меню"
						break;
				}
				break;
			case MAIN_MENU_STATE_MM3: // Выбор пункта 3
				strcpy_P(str_prog16, MMenuItem3); // Копируем строку из flash в буффер ОЗУ
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
					sprintf(str_disp,"%s%3uкВт*ч",str_prog16,power_cons); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"%s   кВт*ч",str_prog16); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Select:
						power_cons = 0; // Сбрасываем счетчик электроэнергии
						main_menu_state = MAIN_MENU_STATE_MM3_IDLE; // Установить статус меню: "Пункт 3 главного меню"
						break;
					case Butt_Esc:
						main_menu_state = MAIN_MENU_STATE_MM3_IDLE; // Установить статус меню: "Пункт 3 главного меню"
						break;
				}
				break;
		}
	}
}

//-----------------------------------------------------
void SubmenuClockProcess(void) // Функция подменю "Часы"
{ 
	// Сохраняем текущие значения во временные переменные
	int8_t mday_tmp = mday,
		   month_tmp = month,
		   year_tmp = year,
		   wday_tmp = wday,
		   hour_tmp = hour,
		   minute_tmp = minute;
	while (TRUE)
	{
		switch (submenu_clock_state) // Смотрим на статус подменю
		{
			case SUBMENU_CLOCK_STATE_IDLE: // Пункт подменю "Дата и время"
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // Копируем строку из flash в буффер ОЗУ
				sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
				direction_str_disp(str_disp); // Вывод пункта подменю "Дата и время"
				submenu_clock_state = SUBMENU_CLOCK_STATE_HOUR; // Установить статус меню: "Редактирование часа"
				break;
			case SUBMENU_CLOCK_STATE_HOUR: // Редактирование часа
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // Копируем строку из flash в буффер ОЗУ
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TICK, то вывести значение
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"  :%02u %s %2u%s%02u",minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						hour_tmp--; // Уменьшаем
						if(hour_tmp<0) hour_tmp = 23;
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						hour_tmp++; // Увеличиваем
						if(hour_tmp>23) hour_tmp = 0;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MINUTE; // Установить статус меню: "Редактирование минуты"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_YEAR; // Установить статус меню: "Редактирование года"
						break;
					case Butt_Select:
						hour = hour_tmp;     // Сохраняем измененное значение
						minute = minute_tmp; // Сохраняем измененное значение
						wday = wday_tmp;     // Сохраняем измененное значение
						mday = mday_tmp;     // Сохраняем измененное значение
						month = month_tmp;   // Сохраняем измененное значение
						year = year_tmp;     // Сохраняем измененное значение
						second = 0; // Обнуляем секунды
						DS1307_Set_Time(); // Установить время (Запуск часов)
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_MINUTE: // Редактирование минуты
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // Копируем строку из flash в буффер ОЗУ
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TICK, то вывести значение
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"%2u:   %s %2u%s%02u",hour_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						minute_tmp--; // Уменьшаем
						if(minute_tmp<0) minute_tmp = 59;
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						minute_tmp++; // Увеличиваем
						if(minute_tmp>59) minute_tmp = 0;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_WDAY; // Установить статус меню: "Редактирование дня недели"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_HOUR; // Установить статус меню: "Редактирование часа"
						break;
					case Butt_Select:
						hour = hour_tmp;     // Сохраняем измененное значение
						minute = minute_tmp; // Сохраняем измененное значение
						wday = wday_tmp;     // Сохраняем измененное значение
						mday = mday_tmp;     // Сохраняем измененное значение
						month = month_tmp;   // Сохраняем измененное значение
						year = year_tmp;     // Сохраняем измененное значение
						second = 0; // Обнуляем секунды
						DS1307_Set_Time(); // Установить время (Запуск часов)
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_WDAY: // Редактирование дня недели
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // Копируем строку из flash в буффер ОЗУ
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TICK, то вывести значение
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"%2u:%02u    %2u%s%02u",hour_tmp,minute_tmp,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						wday_tmp--; // Уменьшаем
						if(wday_tmp<1) wday_tmp = 7;
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						wday_tmp++; // Увеличиваем
						if(wday_tmp>7) wday_tmp = 1;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MDAY; // Установить статус меню: "Редактирование дня месяца"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MINUTE; // Установить статус меню: "Редактирование минуты"
						break;
					case Butt_Select:
						hour = hour_tmp;     // Сохраняем измененное значение
						minute = minute_tmp; // Сохраняем измененное значение
						wday = wday_tmp;     // Сохраняем измененное значение
						mday = mday_tmp;     // Сохраняем измененное значение
						month = month_tmp;   // Сохраняем измененное значение
						year = year_tmp;     // Сохраняем измененное значение
						second = 0; // Обнуляем секунды
						DS1307_Set_Time(); // Установить время (Запуск часов)
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_MDAY: // Редактирование дня месяца
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // Копируем строку из flash в буффер ОЗУ
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TICK, то вывести значение
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"%2u:%02u %s   %s%02u",hour_tmp,minute_tmp,str_prog3,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						mday_tmp--; // Уменьшаем
						if(mday_tmp<1) mday_tmp = 31;
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						mday_tmp++; // Увеличиваем
						if(mday_tmp>31) mday_tmp = 1;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MONTH; // Установить статус меню: "Редактирование месяца"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_WDAY; // Установить статус меню: "Редактирование дня недели"
						break;
					case Butt_Select:
						hour = hour_tmp;     // Сохраняем измененное значение
						minute = minute_tmp; // Сохраняем измененное значение
						wday = wday_tmp;     // Сохраняем измененное значение
						mday = mday_tmp;     // Сохраняем измененное значение
						month = month_tmp;   // Сохраняем измененное значение
						year = year_tmp;     // Сохраняем измененное значение
						second = 0; // Обнуляем секунды
						DS1307_Set_Time(); // Установить время (Запуск часов)
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_MONTH: // Редактирование месяца
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // Копируем строку из flash в буффер ОЗУ
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TICK, то вывести значение
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"%2u:%02u %s %2u   %02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						month_tmp--; // Уменьшаем
						if(month_tmp<1) month_tmp = 12;
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						month_tmp++; // Увеличиваем
						if(month_tmp>12) month_tmp = 1;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_YEAR; // Установить статус меню: "Редактирование года"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MDAY; // Установить статус меню: "Редактирование дня месяца"
						break;
					case Butt_Select:
						hour = hour_tmp;     // Сохраняем измененное значение
						minute = minute_tmp; // Сохраняем измененное значение
						wday = wday_tmp;     // Сохраняем измененное значение
						mday = mday_tmp;     // Сохраняем измененное значение
						month = month_tmp;   // Сохраняем измененное значение
						year = year_tmp;     // Сохраняем измененное значение
						second = 0; // Обнуляем секунды
						DS1307_Set_Time(); // Установить время (Запуск часов)
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_YEAR: // Редактирование года
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // Копируем строку из flash в буффер ОЗУ
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // Копируем строку из flash в буффер ОЗУ
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TICK, то вывести значение
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"%2u:%02u %s %2u%s",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						year_tmp--; // Уменьшаем
						if(year_tmp<0) year_tmp = 99;
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						year_tmp++; // Увеличиваем
						if(year_tmp>99) year_tmp = 0;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_HOUR; // Установить статус меню: "Редактирование часа"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MONTH; // Установить статус меню: "Редактирование месяца"
						break;
					case Butt_Select:
						hour = hour_tmp;     // Сохраняем измененное значение
						minute = minute_tmp; // Сохраняем измененное значение
						wday = wday_tmp;     // Сохраняем измененное значение
						mday = mday_tmp;     // Сохраняем измененное значение
						month = month_tmp;   // Сохраняем измененное значение
						year = year_tmp;     // Сохраняем измененное значение
						second = 0; // Обнуляем секунды
						DS1307_Set_Time(); // Установить время (Запуск часов)
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // Установить статус меню: "Пункт 4 главного меню"
						return;
				}
				break;
		}
	}
}

//-----------------------------------------------------
void SubmenuMemoryProcess(void) // Функция подменю "Память:"
{ 
	uint8_t yes_flag = FALSE; // Флаг поднимается, если выбирается пункт "Да" и опускается, если выбирается пункт "Нет".
	uint8_t cpy_erase = COPY; // Если выбираем пункт "Копирование" cpy_erase = COPY; Если выбираем пункт "Стирание" cpy_erase = EERASE.
	while (TRUE)
	{
		switch (submenu_memory_state) // Смотрим на статус подменю
		{
			case SUBMENU_MEMORY_STATE_M1_IDLE: // Пункт 1 подменю "Память:"
				strcpy_P(str_prog16, MemoryItem1); // Копируем строку из flash в буффер ОЗУ
				direction_str_disp(str_prog16); // Вывод пункта меню "Копировать на SD"
				submenu_memory_state = SUBMENU_MEMORY_STATE_M1_WAIT; // Установить статус меню: "Ожидание выбора 1 пункта"
				break;
			case SUBMENU_MEMORY_STATE_M2_IDLE: // Пункт 2 подменю "Память:"
				strcpy_P(str_prog16, MemoryItem2); // Копируем строку из flash в буффер ОЗУ
				direction_str_disp(str_prog16); // Вывод пункта меню "Очистить память"
				submenu_memory_state = SUBMENU_MEMORY_STATE_M2_WAIT; // Установить статус меню: "Ожидание выбора 2 пункта"
				break;
			case SUBMENU_MEMORY_STATE_M1_WAIT: // Ожидание выбора 1 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Память:""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Память:""
						break;
					case Butt_Select:
						cpy_erase = COPY; // Выбираем пункт "Копирование"
						submenu_memory_state = SUBMENU_MEMORY_STATE_YESNO_WAIT; // Установить статус меню: "Ожидание выбора "Да" или "Нет""
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM7_IDLE; // Установить статус меню: "Пункт 7 главного меню"
						return;
				}
				break;
			case SUBMENU_MEMORY_STATE_M2_WAIT: // Ожидание выбора 2 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Память:""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Память:""
						break;
					case Butt_Select:
						cpy_erase = ERASE; // Выбираем пункт "Стирание"
						submenu_memory_state = SUBMENU_MEMORY_STATE_YESNO_WAIT; // Установить статус меню: "Ожидание выбора "Да" или "Нет""
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM7_IDLE; // Установить статус меню: "Пункт 7 главного меню"
						return;
				}
				break;
			case SUBMENU_MEMORY_STATE_YESNO_WAIT: // Ожидание выбора "Да" или "Нет"
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if (yes_flag){ // Если выбран пункт "Да"
					// Мигаем пунктом "Да":
					strcpy_P(str_prog16, MemoryItem3); // Копируем строку из flash в буффер ОЗУ
					if(tim_cnt < BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
						hcms_puts(str_prog16); // Вывод строки на дисплей
					}
					else {
						hcms_puts("          Нет"); // Вывод строки на дисплей
					}
				}
				else {  // Если выбран пункт "Нет"
					// Мигаем пунктом "Нет":
					strcpy_P(str_prog16, MemoryItem3); // Копируем строку из flash в буффер ОЗУ
					if(tim_cnt < BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
						hcms_puts(str_prog16); // Вывод строки на дисплей
					}
					else {
						hcms_puts("   Да"); // Вывод строки на дисплей
					}
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Right:
						yes_flag = !yes_flag; // Инвертируем переменную
						break;
					case Butt_Left:
						yes_flag = !yes_flag; // Инвертируем переменную
						break;
					case Butt_Select:
						if (yes_flag) // Если выбран пункт "Да"
						{
							if (cpy_erase == COPY) // Если выбран пункт "Копирование"
								submenu_memory_state = SUBMENU_MEMORY_STATE_COPYPROGRESS; // Установить статус меню: "Копирование"
							else if (cpy_erase == ERASE) // Если выбран пункт "Стирание"
								submenu_memory_state = SUBMENU_MEMORY_STATE_ERASEPROGRESS; // Установить статус меню: "Стирание"
						}
						else // Если выбран пункт "Нет"
						{
							if (cpy_erase == COPY) // Если выбран пункт "Копирование"
								submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Память:""
							else if (cpy_erase == ERASE) // Если выбран пункт "Стирание"
								submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Память:""
						}
						yes_flag = FALSE; // Опускаем флаг
						break;
					case Butt_Esc:
						if (cpy_erase == COPY) // Если выбран пункт "Копирование"
							submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Память:""
						else if (cpy_erase == ERASE) // Если выбран пункт "Стирание"
							submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Память:""
						yes_flag = FALSE; // Опускаем флаг
						break;
				}
				break;
			case SUBMENU_MEMORY_STATE_COPYPROGRESS: // Копирование
				// Отправляем байт на SD-карту по адресу 0
				// Читаем байт с SD-карты по адресу 0
				// Если байт прочитан верно (означает, что SD-карта присутствует в слоте)
					// Удаляем этот байт с SD-карты
					// Приступаем к копированию
					// Выводим статус копирования "Прогресс: 100%"
					// Если копирование произошло успешно
						// Выводим сообщение "  Скопировано   "
						// Переходим к пункту меню "Ждем нажатия любой кнопки"
					// Иначе, если копирование произошло НЕ успешно
						// Выводим сообщение "   Copy error   "
						// Переходим к пункту меню "Ждем нажатия любой кнопки"
				// Иначе, если нет SD-карты
					// Выводим сообщение "Нет SD карты"
					// Переходим к пункту меню "Ждем нажатия любой кнопки"
					
					strcpy_P(str_prog16, MemoryItem6); // Копируем строку из flash в буффер ОЗУ
					hcms_puts(str_prog16); // Выводим сообщение "Нет SD карты"
					submenu_memory_state = SUBMENU_MEMORY_STATE_WAIT; // Установить статус меню: "Ждем нажатия любой кнопки"
				break;
			case SUBMENU_MEMORY_STATE_WAIT: // Ждем нажатия любой кнопки
				if(KEYB_GetKey()) // Если произошло нажатие любой кнопки
				{
					if (cpy_erase == COPY) // Если выбран пункт "Копирование"
						submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Память:""
					else if (cpy_erase == ERASE) // Если выбран пункт "Стирание"
						submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Память:""
				}
				break;
			case SUBMENU_MEMORY_STATE_ERASEPROGRESS: // Стирание
				// Выводим статус стирания "Прогресс: 100%"
				// Записываем все 0x55
				// Если запись прошла успешно
					// Читаем память
					// Если память прочитана успешно
						// Стираем память
						// Если стирание произошло успешно
							// Если повреждено больше 0 b
								// Выводим сообщение "Повреждено 999 b"
								// Переходим к пункту меню "Ждем нажатия любой кнопки"
							// Иначе
								// Выводим сообщение "Память очищена"
								// Переходим к пункту меню "Ждем нажатия любой кнопки"
						// Иначе
							// Выводим сообщение "Bad memory"
							// Переходим к пункту меню "Ждем нажатия любой кнопки"
					// Иначе
						// Выводим сообщение "Bad memory"
						// Переходим к пункту меню "Ждем нажатия любой кнопки"
				// Иначе
					// Выводим сообщение "Bad memory"
					// Переходим к пункту меню "Ждем нажатия любой кнопки"
					
					strcpy_P(str_prog16, MemoryItem8); // Копируем строку из flash в буффер ОЗУ
					hcms_puts(str_prog16); // Выводим сообщение "   Bad memory   "
					submenu_memory_state = SUBMENU_MEMORY_STATE_WAIT; // Установить статус меню: "Ждем нажатия любой кнопки"
				break;
		}
	}
}

//-----------------------------------------------------
void SubmenuSettingsProcess(void) // Функция подменю "Настройки"
{
	uint8_t brightness_seg_tmp = brightness_seg; // Яркость 7-сегментного индикатора, % (шаг изменения 2 %)
	uint8_t brightness_led_tmp = brightness_led; // Яркость LED-индикатора, % (шаг изменения 2 %)
	uint8_t brightness_led_p = brightness_led_tmp; // Заглушка для моментального изменения яркости LED-индикатора
	uint8_t reference_I_tmp = reference_I; // Измеренный ток откалиброванным амперметром, *100А
	uint8_t reference_U_tmp = reference_U; // Измеренное напряжение откалиброванным вольтметром, В
	uint16_t reference_IADC_tmp = reference_IADC; // Измеренное значение I АЦП при reference_I, бит
	uint16_t reference_UADC_tmp = reference_UADC; // Измеренное значение U АЦП при reference_U, бит
	// Переменные для пункта меню "Калибровка I":
	extern uint16_t temper_realgrad; // Измеренная температура, [2271;65535] *100'C
 	while (TRUE)
	{
		if (brightness_led_p != brightness_led_tmp){ // Если яркость LED-индикатора изменили
			if (brightness_led_tmp < 16){
				hcms_peak_current(PEAK_04_MA); // Установим пиковый ток сегмента 4.0 mA (31% яркости)
				hcms_bright(brightness_led_tmp%16); // Установим яркость
			}
			else if (brightness_led_tmp >= 16 && brightness_led_tmp < 32){
				hcms_peak_current(PEAK_06_MA); // Установим пиковый ток сегмента 6.4 mA (50% яркости)
				hcms_bright(brightness_led_tmp%16); // Установим яркость
			}
			else if (brightness_led_tmp >= 32 && brightness_led_tmp < 48){
				hcms_peak_current(PEAK_09_MA); // Установим пиковый ток сегмента 9.3 mA (73% яркости)
				hcms_bright(brightness_led_tmp%16); // Установим яркость
			}
			else if (brightness_led_tmp >= 48 && brightness_led_tmp < 64){
				hcms_peak_current(PEAK_13_MA); // Установим пиковый ток сегмента 12.8 mA (100% яркости)
				hcms_bright(brightness_led_tmp%16); // Установим яркость
			}
			brightness_led_p = brightness_led_tmp; // Заглушка
		}
 		switch (submenu_settings_state) // Смотрим на статус подменю
 		{
 			case SUBMENU_SETTINGS_STATE_M1_IDLE: // Пункт 1 подменю "Настройки"
				strcpy_P(str_prog16, SettingsItem1); // Копируем строку из flash в буффер ОЗУ
				direction_str_disp(str_prog16); // Вывод пункта меню "Яркость 7-сегм."
				submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_WAIT; // Установить статус меню: "Ожидание выбора 1 пункта"
				break;
 			case SUBMENU_SETTINGS_STATE_M2_IDLE: // Пункт 2 подменю "Настройки"
				strcpy_P(str_prog16, SettingsItem2); // Копируем строку из flash в буффер ОЗУ
				direction_str_disp(str_prog16); // Вывод пункта меню "Яркость LED инд."
				submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_WAIT; // Установить статус меню: "Ожидание выбора 2 пункта"
				break;
 			case SUBMENU_SETTINGS_STATE_M3_IDLE: // Пункт 3 подменю "Настройки"
				strcpy_P(str_prog16, SettingsItem3); // Копируем строку из flash в буффер ОЗУ
				direction_str_disp(str_prog16); // Вывод пункта меню "Калибровка U"
				submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_WAIT; // Установить статус меню: "Ожидание выбора 3 пункта"
				break;
			case SUBMENU_SETTINGS_STATE_M4_IDLE: // Пункт 4 подменю "Настройки"
				strcpy_P(str_prog16, SettingsItem4); // Копируем строку из flash в буффер ОЗУ
				direction_str_disp(str_prog16); // Вывод пункта меню "Калибровка I"
				submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_WAIT; // Установить статус меню: "Ожидание выбора 4 пункта"
				break;
			case SUBMENU_SETTINGS_STATE_M1_WAIT: // Ожидание выбора 1 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Настройки""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_IDLE; // Установить статус меню: "Пункт 4 подменю "Настройки""
						break;
					case Butt_Select:
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1; // Установить статус меню: "Выбор пункта 1"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // Установить статус меню: "Пункт 8 главного меню"
						return;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M2_WAIT: // Ожидание выбора 2 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_IDLE; // Установить статус меню: "Пункт 3 подменю "Настройки""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Настройки""
						break;
					case Butt_Select:
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2; // Установить статус меню: "Выбор пункта 2"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // Установить статус меню: "Пункт 8 главного меню"
						return;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M3_WAIT: // Ожидание выбора 3 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_IDLE; // Установить статус меню: "Пункт 4 подменю "Настройки""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Настройки""
						break;
					case Butt_Select:
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3; // Установить статус меню: "Выбор пункта 3"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // Установить статус меню: "Пункт 8 главного меню"
						return;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M4_WAIT: // Ожидание выбора 4 пункта
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // Переходим вниз
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Настройки""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // Переходим вверх
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_IDLE; // Установить статус меню: "Пункт 3 подменю "Настройки""
						break;
					case Butt_Select:
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4; // Установить статус меню: "Выбор пункта 4"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // Переходим налево
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // Установить статус меню: "Пункт 8 главного меню"
						return;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M1: // Выбор пункта 1
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
					sprintf(str_disp,"%3u%%",brightness_seg_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					hcms_puts("   %"); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if (brightness_seg_tmp > 0)
							brightness_seg_tmp -= 2; // Уменьшаем яркость
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if (brightness_seg_tmp < 100)
							brightness_seg_tmp += 2; // Увеличиваем яркость
						break;
					case Butt_Select:
						brightness_seg = brightness_seg_tmp; // Сохраняем измененное значение яркости
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Настройки""
						break;
					case Butt_Esc:
						brightness_seg_tmp = brightness_seg; // Возвращаем исходное заначение яркости во временную переменную
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // Установить статус меню: "Пункт 1 подменю "Настройки""
						break;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M2: // Выбор пункта 2
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
					sprintf(str_disp,"%3u%%",brightness_led_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					hcms_puts("   %"); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if (brightness_led_tmp > 0)
							brightness_led_tmp --; // Уменьшаем яркость
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if (brightness_led_tmp < 63)
							brightness_led_tmp ++; // Увеличиваем яркость
						break;
					case Butt_Select:
						brightness_led = brightness_led_tmp; // Сохраняем измененное значение яркости
						EEPROM_write_byte(ADDREEP_BR_LED, brightness_led_tmp); // Записать байт в EEPROM по адресу
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Настройки""
						break;
					case Butt_Esc:
						brightness_led_tmp = brightness_led; // Возвращаем исходное заначение яркости во временную переменную
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_IDLE; // Установить статус меню: "Пункт 2 подменю "Настройки""
						break;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M3: // Выбор пункта 3
				reference_UADC_tmp = adc_data(ADC_CH_2); // Читаем значение АЦП (напряжение)
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
					sprintf(str_disp,"U=%3uВ  ADC=%u",reference_U_tmp,reference_UADC_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"U=   В  ADC=%u",reference_UADC_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if (reference_U_tmp > 0)
							reference_U_tmp--; // Уменьшаем яркость
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if (reference_U_tmp < EEP_CLEN_BYTE - 1) // т.к. 255 - пустая ячейка в EEPROM
							reference_U_tmp++; // Увеличиваем значение измеренного напряжения
						break;
					case Butt_Select:
						reference_U = reference_U_tmp; // Сохраняем измененное значение измеренного напряжения
						reference_UADC = reference_UADC_tmp; // Сохраняем измененное значение АЦП
						EEPROM_write_byte(ADDREEP_REF_U, reference_U_tmp); // Записать байт в EEPROM по адресу
						EEPROM_write_word(ADDREEP_REF_UADC, reference_UADC_tmp); // Записать слово (2 байта) в EEPROM по адресу
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_IDLE; // Установить статус меню: "Пункт 3 подменю "Настройки""
						break;
					case Butt_Esc:
						reference_U_tmp = reference_U; // Возвращаем исходное заначение измеренного напряжения во временную переменную
						reference_UADC_tmp = reference_UADC; // Возвращаем исходное заначение измеренного значения АЦП во временную переменную
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_IDLE; // Установить статус меню: "Пункт 3 подменю "Настройки""
						break;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M4: // Выбор пункта 4
				temper_realgrad = pgm_read_word(&table_termores[adc_data(ADC_CH_1)]); // Вычисляем температуру 'C по измеренному значению АЦП
				if (temper_realgrad < 12000){ // Если чаша не нагрелась до 120 'C
					TEN_ON; // Включаем ТЭН
					LED_TEN_ON; // Включаем светодиод ТЭН
				}
				else {
					TEN_OFF; // Выключаем ТЭН
					LED_TEN_OFF; // Выключаем светодиод ТЭН
				}
				reference_IADC_tmp = adc_data(ADC_CH_0); // Читаем значение АЦП (ток)
				//-----------------МИГАЕМ ЗНАЧЕНИЕМ--------------------
				if(tim_cnt<BURNING_TICK) { // Если счетчик меньше BURNING_TIME, то вывести значение
					sprintf(str_disp,"I=%01u.%02uА ADC=%u",reference_I_tmp/100,reference_I_tmp%100,reference_IADC_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				else {
					sprintf(str_disp,"I=    А ADC=%u",reference_IADC_tmp); // Запись строки в буфер
					hcms_puts(str_disp); // Вывод строки на дисплей
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if (reference_I_tmp > 0)
							reference_I_tmp--; // Уменьшаем значение измеренного тока
						break;
					case Butt_Up:
						but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
						if (reference_I_tmp < EEP_CLEN_BYTE - 1) // т.к. 255 - пустая ячейка в EEPROM
							reference_I_tmp++; // Увеличиваем значение измеренного тока
						break;
					case Butt_Select:
						TEN_OFF; // Выключаем ТЭН
						LED_TEN_OFF; // Выключаем светодиод ТЭН
						reference_I = reference_I_tmp; // Сохраняем измененное значение измеренного тока
						reference_IADC = reference_IADC_tmp; // Сохраняем измененное значение АЦП
						EEPROM_write_byte(ADDREEP_REF_I, reference_I_tmp); // Записать байт в EEPROM по адресу
						EEPROM_write_word(ADDREEP_REF_IADC, reference_IADC_tmp); // Записать слово (2 байта) в EEPROM по адресу
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_IDLE; // Установить статус меню: "Пункт 4 подменю "Настройки""
						break;
					case Butt_Esc:
						TEN_OFF; // Выключаем ТЭН
						LED_TEN_OFF; // Выключаем светодиод ТЭН
						reference_I_tmp = reference_I; // Возвращаем исходное заначение измеренного напряжения во временную переменную
						reference_IADC_tmp = reference_IADC; // Возвращаем исходное заначение измеренного значения АЦП во временную переменную
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_IDLE; // Установить статус меню: "Пункт 4 подменю "Настройки""
						break;
				}
				break;
 		}
 	}
}