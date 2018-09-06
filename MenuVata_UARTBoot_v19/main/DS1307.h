#ifndef RTC_H_
#define RTC_H_

#include "main.h"

#define DS1307_ADDR 0b11010000 // Адрес устройства (состоит из 7 бит + восьмой бит (бит квитирования - R/W))

// НЕ ВЫЗЫВАТЬ ФУНКЦИИ В ОБРАБОТЧИКЕ ПРЕРЫВАНИЙ!!!!!
void DS1307_Set_Time(void); // Установить время (Запуск часов)
void DS1307_Read_Time(void); // Прочитать время в переменные mday..second

#endif /* RTC_H_ */