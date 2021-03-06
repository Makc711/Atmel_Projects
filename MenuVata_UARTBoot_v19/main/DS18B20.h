﻿#ifndef DS18B20_H_
#define DS18B20_H_

#include "main.h"

#define NOID 0xCC // Пропустить идентификацию (т.к. используется одно устройство на шине 1-wire)
#define T_CONVERT 0x44 // Код измерения температуры
#define READ_DATA 0xBE // Передача байтов ведущему

#define DISCRDS18 0.0625 // Дискретность измерения температуры
#define KDISCRDS (DISCRDS18*10000) // Коэф-т дискретности

// Задержка в 750 мс при измерении температуры выполняется в таймере 0 (находится в keyboard.c)

uint16_t dt_check(void); // Функция преобразования показаний датчика в температуру
uint16_t dt_converttemp(uint16_t tt); // Преобразование температуры в единицы [00000;12500] (2 последних значения - 2 знака после запятой)

#endif /* DS18B20_H_ */