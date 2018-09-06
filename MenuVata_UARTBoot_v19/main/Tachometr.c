﻿#include "Tachometr.h"

//-----------------Объявление переменных---------------------
uint8_t t2_0=0, t2_1=0; // Количество предыдущих и текущих тиков таймера-счетчика 2
uint16_t t2_cycles=0; // Количество прерываний теймера-счетчика 2 c момента прыдыдущего внешнего прерывания по нарастающему фронту
uint32_t t2_ticks_int=0; // Количество тиков таймера-счетчика 2 за полный оборот
uint8_t flag_int=TRUE; // Флаг для прерываний
uint16_t rpm=0; // Частота вращения, об/мин
uint16_t rpmX=0; // Усредненное значение измеренной частоты вращения
uint8_t rpmn=0; // Счетчик измерений RPM
uint16_t get_rpm=0; // Измеренная частота вращения, об/мин
extern uint16_t rpm_out; // Частота вращения, отправляемая на регулятор, об/мин [200;2000]
uint8_t tpwm=0; // Время отклика ШИМ
uint8_t pwm_mot=0; // Уровень ШИМ для МОТОРА [0;255]
uint8_t trpm=0; // Делитель таймера для вычисления RPM

INIT(8){ // Инициализация Тахометра
	//--------------Инициализация внешних прерываний-------------
	DDR(PORT_INT4) &= ~_BV(PIN_INT4); // Установка линии INT4_PIN_NUM порта INT4_DDR на вход
	EICRB |= (1<<ISC41)|(1<<ISC40); // Задаем прерывания INT4 по восходящему фронту
	//-----------------------------------------------------------
	//-------------------Инициализация таймера-------------------
  	TCCR2 |= (0<<CS22)|(1<<CS21)|(1<<CS20); // Устанавливаем предделитель на 64
  								  // таймер-счетчик 2 считает от 0x00 до 0xFF
	//-----------------------------------------------------------
}
//========================ПРЕРЫВАНИЯ=========================
//--------------------Внешнее прерывание---------------------
ISR (INT4_vect){
	cli();
	// Вычисляем количество тиков таймера за один оборот
	if (!flag_int){ // Если флаг для прерываний сброшен
		t2_0 = t2_1; // Записать в t2_0 предыдущее значение TCNT2
		t2_1 = TCNT2; // Записать в t2_1 текущее значение TCNT2
		t2_ticks_int = (uint32_t)t2_cycles * TICK_INT_T2 + t2_1 - t2_0; // Количество тиков таймера-счетчика 2 за полный оборот
		if (t2_ticks_int <= F_CPU*60/PRESCALER_T2/RPM_MAX) t2_ticks_int = F_CPU*60/PRESCALER_T2/RPM_MAX; // Ограничение по максимальной частоте вращения
		t2_cycles = 0; // Сбросим счетчик прерываний таймера-счетчика 0
	}
	else { // Если флаг для прерываний установлен
		t2_1 = TCNT2; // Записать в t2_1 текущее значение TCNT2
		t2_cycles = 0; // Сбросим счетчик прерываний таймера-счетчика 2
		t2_ticks_int = 0; // Сбросим счетчик тиков таймера-счетчика 2 за полный оборот
		flag_int = FALSE; // Сбросить флаг для прерываний
	}
	sei();
}
//--------------------Прерывание таймера2---------------------
ISR (TIMER2_OVF_vect){
	//------Прерывание по переполнению таймера-счетчика 2--------
	// FT2=7372800/64/256=450 раз в секунду происходят прерывания Т2
	// Кол-во прерываний T2 на 1 оборот = FT2/RPM(об/сек)= 450*60/30=900 прерываний/оборот при RPM_MIN
	//													 t2_cycles < 900
	if (t2_cycles < F_CPU*60/PRESCALER_T2/RPM_MIN/TICK_INT_T2){ // Если с момента последнего внешнего прерывания прошло меньше ~2 секунд
		t2_cycles++; // +1 когда выполняется прерывание таймера-счетчика 2
	}
	else { // Если ~2 секунды нет внешнего прерывания по нарастающему фронту (не сбрасывалась t2_cycles)
		flag_int = TRUE; // Ограничение по минимальной частоте вращения
	}
	//-------РЕГУЛИРОВАНИЕ RPM-------
	if (tpwm++ == TPWM){ // Настраиваем время отклика
		tpwm = 0;
		if (get_rpm < rpm_out - GPWM){ // Если измеренное значение RPM меньше установленного (-гистерезис)
			if (pwm_mot < 255) // Ограничение по максимальному значению
				pwm_mot++;
		}
		else if (get_rpm > rpm_out + GPWM){ // Если измеренное значение RPM больше установленного (+гистерезис)
			if (pwm_mot >= 1) // Ограничение по минимальному значению 
				pwm_mot--;
		}
		OCR1B = pwm_mot; // Записываем значение ШИМ (МОТОР)
	}
	//-------------------------------
	if (trpm++ == TRPM){ // Настраиваем период вычисления RPM
		trpm = 0;
		// Вычисляем частоту вращения
		// Два условия:
		// 1) Для ограничания rpm, пока t2_ticks_int=0, иначе будет переполнение rpm
		// 2) Ограничение по минимальной частоте вращения
		if (t2_ticks_int >= F_CPU*60/PRESCALER_T2/RPM_MAX && !flag_int) // Ограничение по максимальной и минимальной частоте вращения
			rpm = F_CPU*60/PRESCALER_T2/t2_ticks_int; // 60 - секунд в минуте (используем, т.к. RPM [об/мин])
		else // Вал остановился, долго не было прерываний
			rpm = 0; // Обнуляем текущее мгновенное значение RPM
		rpmX += rpm; // Сумма RPM_NUM измерений частоты вращения
		rpmn++;
		if (rpmn == RPM_NUM){ // Делаем RPM_NUM измерений
			rpmn = 0; // Обнуляем счетчик RPM
			rpmX = rpmX / RPM_NUM; // Считаем среднее арифметическое RPM
			get_rpm = rpmX; // Получаем среднее значение RPM
			rpmX = 0; // Обнуляем среднее значение RPM
		}
	}
}
//===========================================================