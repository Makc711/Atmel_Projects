#include "main.h"
//===============ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ===============
uint8_t mday = 1, // День месяца [1;31]
		month = 1, // Месяц [1;12]
		year = 17, // Год [0;99]
		wday = 1, // День недели [1;7]
		hour = 12, // Час [0;24]
		minute = 0, // Минута [0;59]
		second = 0; // Секунда [0;59]
uint8_t DS1307_Read_flag = FALSE; // Флаг для чтения времени из DS1307 (FALSE - не читаем; TRUE - читаем)
uint16_t temper_set = 6000; // Установленная температура, [15000;25000] *100'C
uint16_t rpm_set = 400, // Установленная частота вращения, об/мин [200;2000]
		 rpm_slow = 300, // Частота Медленного вращения (до нагрева)
		 rpm_out = 0; // Частота вращения, отправляемая на регулятор, об/мин [200;2000]
uint16_t temper_realADC = 0; // Измеренная температура, бит [0;1023]
uint16_t temper_realgrad = 0; // Измеренная температура, [2271;65535] *100'C
uint16_t voltage_ADC = 0; // Напряжение сети, бит [0;1023]
uint16_t current_ADC = 0; // Общий ток, бит [0;1023]
uint8_t voltage = 0; // Напряжение сети, В
uint8_t current_total = 0; // Общий ток, *10А
uint16_t power_cons = 0; // Расход мощности, кВт*ч
uint8_t change = FALSE; // Флаг для индикации температуры/RPM на 7-сег индикаторе (FALSE - темп./ TRUE - RPM)
extern uint8_t button_state; // Код нажатой кнопки
//------Переменные для тахометра------
extern uint8_t t2_1; // Количество текущих тиков таймера-счетчика 2
extern uint16_t t2_cycles; // Количество прерываний теймера-счетчика 2 c момента прыдыдущего внешнего прерывания по нарастающему фронту
extern uint32_t t2_ticks_int; // Количество тиков таймера-счетчика 2 за полный оборот
extern uint8_t flag_int; // Флаг для прерываний
extern uint16_t rpm; // Измеренная частота вращения, об/мин (мгновенное значение)
extern float rpmX; // Усредненное значение измеренной частоты вращения, об/мин
extern uint16_t get_rpm; // Измеренная частота вращения, об/мин
//-------Переменные для мотора--------
extern uint8_t pwm_mot; // Уровень ШИМ для МОТОРА [0;255]
//------------------------------------
uint16_t rpm_lcd = 0; // Частота вращения, округленная до сотен, об/мин
uint8_t temper_lcd = 0; // Температура, округленная до единиц, 'C
char bufled[4]; // Буфер для вывода информации на 7-сегментные индикаторы (4 символа)
uint8_t seccnt = 0; // Счетчик - нужен для мигания ":" в часах
uint8_t secstatus = FALSE; // Состояние ":" в часах (FALSE - погашен/TRUE - горит)
uint8_t start = FALSE; // Флаг запуска работы (FALSE - СТОП/TRUE - СТАРТ)
//------------------------------------
uint16_t temper_realDS = 0; // Переменная для хранения внешней температуры [00000;12500] *100'C
extern uint8_t pwm_ten; // Уровень ШИМ [0;100] (ТЭН)
//----------ПЕРЕМЕННЫЕ ДЛЯ ПИД (ТЕМПЕРАТУРА)---------
int16_t e = 0; // Текущее значение ошибки, *100'C
extern float ef; // Текущее значение ошибки, 'C
extern float integral; // Интегральная составляющая
extern float e1; // Первый такт ошибки (текущая ошибка)
//-----------ПЕРЕМЕННЫЕ ДЛЯ ИСТОРИИ РАБОТЫ-----------
uint8_t secdata = 0; // Счетчик секунд между отправляемыми сообщениями во внешнюю EEPROM
uint16_t addrdata = 0; // Адрес ячейки памяти, в которую записывается сообщение
uint8_t writeDataEEPext = FALSE; // Флаг для записи сообщения во внешнюю EEPROM (FALSE - не пишем; TRUE - пишем)
uint8_t dataEEPext[16]; // Сообщение, передаваемое во внешнюю EEPROM
uint8_t twEEPextState = TWI_SUCCESS; // Статус модуля TWI
uint8_t errorEEPext = FALSE; // Флаг поднимается, если внешняя EEPROM память неисправна (слишком много ошибок записи)
uint16_t freememory = FREE_EEPEXT; // Свободная EEPROM память *10, %
uint8_t addrEEPint = ADDREEP_BAD_EEPEXT_FIRST; // Адрес ячеки памяти внутренней EEPROM, в которую записывается адрес ячейки памяти внешней EEPROM при неудавшейся записи
//-------ПЕРЕМЕННЫЕ ДЛЯ ПУНКТА МЕНЮ "Настройки"------
uint8_t brightness_seg = 50; // Яркость 7-сегментного индикатора, %
uint8_t brightness_led = 50; // Яркость LED-индикатора, % (шаг изменения 2 %)
uint8_t reference_I = 100; // Измеренный ток откалиброванным амперметром, *100А
uint8_t reference_U = 220; // Измеренное напряжение откалиброванным вольтметром, В
uint16_t reference_IADC = 150; // Измеренное значение I АЦП при reference_I, бит
uint16_t reference_UADC = 900; // Измеренное значение U АЦП при reference_U, бит

extern uint8_t but_press; // Флаг для многократного "нажатия" на кнопку при ее удержании (0-нет многократного нажатия, иначе - есть)
uint8_t flag_set_led = FALSE; // Флаг для индикации 7-сег индикатора (TRUE - установленное значение; FALSE - измеренное значение)
//===================================================

//-----------------------------------------------------
INIT(7){ 
	// Инициализация внешних прерываний для часов:
    EICRA |= (1<<ISC21); // Включим прерывания INT2 по нисходящему фронту
    EIMSK |= (1<<INT2); // Разрешим внешние прерывания INT2
	// Инициализация светодиодов ТЭН, ПУСК:
	DDR(PORT_LED) |= _BV(PIN_LED_TEN) | _BV(PIN_LED_START); // Пины порта PORT_LED на выход
}
//-----------------------------------------------------
ISR(INT2_vect){ // Внешнее прерывание INT2
	DS1307_Read_flag = TRUE; // Читаем время в переменные mday..second
	seccnt = 0; // Обнуляем счетчик (мигалка ":" в часах)
	secstatus = FALSE; // Гасим ":" в часах
	power_cons += voltage * current_total / 1000; // Рассчитаываем затраченную мощность (кВт*с)
	if (start){ // Если была команда СТАРТ (флаг поднят)
		if (secdata == 0){
			dataEEPext[0] = 0;
			dataEEPext[1] = mday; // День месяца [1;31]
			dataEEPext[2] = month; // Месяц [1;12]
			dataEEPext[3] = year; // Год [0;99]
			dataEEPext[4] = wday; // День недели [1;7]
			dataEEPext[5] = hour; // Час [0;24]
			dataEEPext[6] = minute; // Минута [0;59]
			dataEEPext[7] = second; // Секунда [0;59]
			dataEEPext[8] = temper_set/100; // Установленная температура, [150;250] 'C
			dataEEPext[9] = rpm_set/100; // Установленная частота вращения, [2;20] /100об/мин 
			dataEEPext[10] = temper_lcd; // Температура, округленная до единиц, [0;255] 'C
			dataEEPext[11] = rpm_lcd/100; // Частота вращения, округленная до сотен, [0;20] /100об/мин
			dataEEPext[12] = temper_realDS/100; // Внешняя температура, [0;125] 'C
			dataEEPext[13] = voltage; // Напряжение сети, [0;255] В
			dataEEPext[14] = current_total; // Общий ток, [0;255] *10А
			dataEEPext[15] = power_cons; // Расход мощности, [0;255] кВт*ч
//			writeDataEEPext = TRUE; // Поднимаем флаг
			secdata = INTERVAL_BW_MSG; // Интервал 30 секунд между пакетами данных
		}
		else secdata--; // Счетчик секунд для выдержки интервала между записываемыми данными во внешнюю EEPROM
	}
}
//-----------------------------------------------------
int main(void){
	sei(); // Разрешаем глобальные прерывания
	
	DS1307_Read_Time(); // Прочитать время в переменные mday..second (при старте)
	
	if (EEPROM_read_word(ADDREEP_TEMPER_SET) != EEP_CLEN_WORD) // Если EEPROM память не стерта
		temper_set = EEPROM_read_word(ADDREEP_TEMPER_SET); // Прочитать установленную температуру нагрева
	if (EEPROM_read_word(ADDREEP_RPM_SET) != EEP_CLEN_WORD)
		rpm_set = EEPROM_read_word(ADDREEP_RPM_SET); // Прочитать установленную частоту вращения
	if (EEPROM_read_byte(ADDREEP_BR_LED) != EEP_CLEN_BYTE)
		brightness_led = EEPROM_read_byte(ADDREEP_BR_LED); // Прочитать установленную яркость LED-индикатора
	if (EEPROM_read_byte(ADDREEP_REF_U) != EEP_CLEN_BYTE)
		reference_U = EEPROM_read_byte(ADDREEP_REF_U); // Прочитать откалиброванное значение U
	if (EEPROM_read_word(ADDREEP_REF_UADC) != EEP_CLEN_WORD)
		reference_UADC = EEPROM_read_word(ADDREEP_REF_UADC); // Прочитать откалиброванное значение UADC
	if (EEPROM_read_byte(ADDREEP_REF_I) != EEP_CLEN_BYTE)
		reference_I = EEPROM_read_byte(ADDREEP_REF_I); // Прочитать откалиброванное значение I
	if (EEPROM_read_word(ADDREEP_REF_IADC) != EEP_CLEN_WORD)
		reference_IADC = EEPROM_read_word(ADDREEP_REF_IADC); // Прочитать откалиброванное значение IADC
	
	while(TRUE){
		MenuProcess(); // Фызов функции меню
		
		if (DS1307_Read_flag){ // Если флаг поднят
			DS1307_Read_Time(); // Прочитать время в переменные mday..second
			DS1307_Read_flag = FALSE; // Опустить флаг
		}
		
		temper_realDS = dt_converttemp(dt_check()); // Измеряем внешнюю температуру [00000;12500] *100'C
		current_ADC = adc_data(ADC_CH_0); // Читаем значение АЦП (ток)
		temper_realADC = adc_data(ADC_CH_1); // Читаем значение АЦП (температура)
		voltage_ADC = adc_data(ADC_CH_2); // Читаем значение АЦП (напряжение)
		temper_realgrad = pgm_read_word(&table_termores[temper_realADC]); // Вычисляем температуру 'C по измеренному значению АЦП
		//------------ПИД--------------------------
		e = temper_set - temper_realgrad; // Ошибка (установленная-измеренная температура)
		if (abs(e) > INTEGRAL_GIST_GRAD) // Если разность температур больше 2 'C
			integral = 0; // Обнуляем интеграл
		ef = (float) e / 100; // Переводим *100'C в 'C
		//-----------------------------------------
		if (start){ // Если была команда СТАРТ (флаг поднят)
			if (change){ // Если производится индикация температуры
				if (button_state == Butt_Up){ // Если произошло нажатие кнопки ВВЕРХ
					but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
					flag_set_led = TRUE; // Выводим на 7-сег индикатор установленное значение
					if (temper_set < TEMPER_SETMAX) // Ограничение по максимальной температуре нагрева
						temper_set += 100; // Увеличиваем температуру нагрева
				}
				else if (button_state == Butt_Down){ // Если произошло нажатие кнопки ВНИЗ
					but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
					flag_set_led = TRUE; // Выводим на 7-сег индикатор установленное значение
					if (temper_set > TEMPER_SETMIN) // Ограничение по минимальной температуре нагрева
						temper_set -= 100; // Уменьшаем температуру нагрева
				}
			}
			else { // Если производится индикация частоты вращения
				if (button_state == Butt_Up){ // Если произошло нажатие кнопки ВВЕРХ
					but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
					flag_set_led = TRUE; // Выводим на 7-сег индикатор установленное значение
					if (rpm_set < RPM_SETMAX) // Ограничение по максимальной температуре нагрева
						temper_set += 100; // Увеличиваем температуру нагрева
				}
				else if (button_state == Butt_Down){ // Если произошло нажатие кнопки ВНИЗ
					but_press = TRUE; // Разрешаем многократное "нажатие" на кнопку
					flag_set_led = TRUE; // Выводим на 7-сег индикатор установленное значение
					if (rpm_set > RPM_SETMIN) // Ограничение по минимальной температуре нагрева
						temper_set -= 100; // Уменьшаем температуру нагрева
				}
			}
			// Пока температура не вырастет до порога плавления сахара, крутим медленно
			if (temper_realgrad < temper_set - RPM_GIST_GRAD) // Если чаша не нагрелась до нужной температуры
				rpm_out = rpm_slow; // Крутим медленно
			else 
				rpm_out = rpm_set; // Крутим нормально
		}
		//-----------------------------------------
		current_total = current_ADC * reference_I / reference_IADC; // Общий ток, *100А | *коэф (измеренный ток, А *100 / current_ADC) (1 А при 150 бит)
		voltage = (uint32_t) voltage_ADC * reference_U / reference_UADC; // Напряжение сети, В | *коэф (измеренное напряжение, В / voltage_ADC) (220 В при 900 бит)
		
		if (change){ // Если установлен флаг
			if (flag_set_led){ // Если нужно вывести установленное значение
				sprintf(bufled,"%4u",rpm_set); // Выводим RPM
				WriteStringMC14489(bufled, H_Alloff); // Отправить строку данных и точку
			}
			else{ // Если нужно вывести измеренное значение
				rpm_lcd = (get_rpm + 50)/100*100; // Округляем до сотен
				sprintf(bufled,"%4u",rpm_lcd); // Выводим RPM
				WriteStringMC14489(bufled, H_Alloff); // Отправить строку данных и точку
			}
		}
		else {
			if (flag_set_led){ // Если нужно вывести установленное значение
				sprintf(bufled,"%3uC",temper_set/100); // Выводим температуру
				WriteStringMC14489(bufled, H_BANK4); // Отправить строку данных и точку
			}
			else{ // Если нужно вывести измеренное значение
				if (temper_realgrad < temper_realDS + 1000) // Если температура нагрева меньше внешней температуры на 10 'C
					temper_lcd = (temper_realDS + 50)/100; // Округляем до единиц
				else
					temper_lcd = (temper_realgrad + 50)/100; // Округляем до единиц
				sprintf(bufled,"%3uC",temper_lcd); // Выводим температуру
				WriteStringMC14489(bufled, H_BANK4); // Отправить строку данных и точку
			}
		}

		if (button_state == Butt_Start){ // Если произошло нажатие кнопки Start
			if (!start){ // Если до этого была команда СТОП
				LED_START_ON; // Включаем светодиод ПУСК
				TIMSK |= (1<<TOIE2); // Разрешаем прерывания при переполнении таймера-счетчика 2
				EIMSK |= (1<<INT4); // Разрешаем внешние прерывания INT4
				pwm_mot = PWMSTA; // Задаем начальное значение ШИМ для быстрого старта мотора
				start = TRUE; // Установим флаг
			}
		}
		else if (button_state == Butt_Stop){ // Если произошло нажатие кнопки Stop
			if (start){ // Если до этого была команда СТАРТ
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
				rpm_out = 0; // Останавливаем мотор
				pwm_mot = 0; // Обнуляем установленное значение ШИМ
				OCR1B = 0; // Сбрасываем ШИМ (МОТОР)
				//_______Выключаем ТЭН________
				pwm_ten = 0; // Устанавливаем ШИМ в 0
				PORT(PORT_PWMTEN) |= _BV(PIN_PWMTEN); // Подаем 1 (гасим ТЭН)
				integral = 0; // Обнуляем интеграл
				e1 = 0; // Обнуляем ошибку
				//____________________________
				secdata = 0; // Обнуляем счетчик, если была команда СТОП
				//____________________________
				start = FALSE; // Снимем флаг
			}
		}
		else if (button_state == Butt_Change){ // Если произошло нажатие кнопки Change
			change = !change; // Флаг для смены индикации температуры/RPM на 7-сег индикаторе
		}
		
		if (writeDataEEPext){ // Если флаг поднят
			twEEPextState = EE_Write_msg(addrdata, dataEEPext, BYTES_TO_EEPEXT); // Записать сообщение во внешнюю EEPROM по адресу
			if (twEEPextState != TWI_SUCCESS){ // Если не удалось записать данные по этому адресу
				EEPROM_write_word(addrEEPint, addrdata); // Сохраним этот адрес в EEPROM
				if (addrEEPint < ADDREEP_BAD_EEPEXT_LAST) // Ограничение по используемой памяти
					addrEEPint += BAD_EEPEXT_BYTE; // Переходим на следующий адрес
				else errorEEPext = TRUE; // Внешняя EEPROM память неисправна
			}
			if (addrdata < (MEMORY-BYTES_TO_EEPEXT)){ // Ограничение по используемой памяти
				addrdata += BYTES_TO_EEPEXT; // Переходим на следующий адрес
				freememory = FREE_EEPEXT - ((addrdata / (MEMORY-BYTES_TO_EEPEXT)) * FREE_EEPEXT); // Вычисляем свободную память *10, %
			}
			writeDataEEPext = FALSE; // Опустить флаг
		}
	}
}
