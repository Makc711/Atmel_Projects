﻿#ifndef TACHOMETR_H_
#define TACHOMETR_H_
//-----------------------------------------------------------
#include "main.h"
//-----------------------------------------------------------
#define RPM_MAX 5000 // Ограничение по максимальной частоте вращения, об/мин
#define RPM_MIN 100 // Ограничение по минимальной частоте вращения, об/мин
#define PRESCALER_T2 64 // Предделитель таймера-счетчика 2
#define TICK_INT_T2 256 // Количество тиков Т2 до прерывания по переполнению (2^8 или 2^16)
#define GPWM 40 // Гистерезис (чтобы обороты не плавали около установленного значения)
#define PWMSTA 55 // Начальное значение ШИМ для быстрого старта мотора
#define TPWM 45 // Время отклика ШИМ (изменение ШИМ каждые 0,1 сек.)
#define TRPM 5 // Делитель таймера для вычисления RPM (вычисления каждые 0,011 сек.)
#define RPM_NUM 12 // Количество измерений RPM для расчета среднего значения
//----------------------------------------
//__________Входы кампаратора_____________
#define PORT_INT4    E // Порт для внешних прерываний INT4
#define PIN_INT4     4 // Пин для внешних прерываний INT4
//----------------------------------------

#endif /* TACHOMETR_H_ */