#ifndef PWM_H_
#define PWM_H_

#include "main.h"
//----------------------------------------
#define PORT_PWMTEN     B // Порт для ШИМ (ТЭН)
#define PIN_PWMTEN      5 // Пин порта для ШИМ (ТЭН)

#define TEN_ON       PORT(PORT_PWMTEN) &= ~_BV(PIN_PWMTEN)  // Подаем 0 (включаем ТЭН)
#define TEN_OFF      PORT(PORT_PWMTEN) |= _BV(PIN_PWMTEN)   // Подаем 1 (выключаем ТЭН)
//----------------------------------------
#define PORT_PWMMOT     B // Порт для ШИМ (МОТОР)
#define PIN_PWMMOT      6 // Пин порта для ШИМ (МОТОР)
//----------------------------------------
#define PRESCALER_T3    8 // Предделитель таймера-счетчика 3
#define FREQ_T3        100 // Частота прерываний таймера-счетчика 3, Гц (раз в секунду)
#define COMPARE_T3     (F_CPU/PRESCALER_T3/FREQ_T3) // Таймер-счетчик 3 производит с этим числом сравнение и вызывает прерывание при совпадении

#endif /* PWM_H_ */