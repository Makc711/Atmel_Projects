#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 7372800UL // Частота тактирования микроконтроллера в Гц

#include <avr/io.h> // Библиотека для работы с AVR микроконтроллерами
/*<avr/io.h>: | keyboardXY.h | hcms_297x.с | */
#include <avr/interrupt.h> // Библиотека для прерываний
/*<avr/interrupt.h>: | main.c | */
#include <util/delay.h> // Библиотека задержек
/*<util/delay.h>: | keyboardXY.h | hcms_297x.с | eepromext.с | */
#include <stdio.h> // Библиотека ввода-вывода (sprintf)
/*<stdio.h>: | usart.с | hcms_297x.с | */
#include <stdlib.h> // Стандартная библиотека
/*<stdlib.h>: | usart.h | twi.с | */
#include <avr/pgmspace.h> // Библиотека для работы с flash памятью программ
/*<avr/pgmspace.h>: | hcms_297x.с | */
#include <stdint.h> // Библиотека пользовательских типов данных
/*<stdint.h>: | usart.с | hcms_297x.с | */
#include <string.h> // Библиотека для работы с нуль-терминированными строками и различными функциями работы с памятью
/*<string.h>: | hcms_297x.с | eepromext.с | */

//============ПОЛЬЗОВАТЕЛЬСКИЕ БИБЛИОТЕКИ============
#include "usart.h" // Библиотека для работы с UART
#include "hcms_297x.h" // Библиотека для работы с матричным LED  индикатором HCMS_297x
#include "keyboardXY.h" // Библиотека клавиатуры
#include "menu.h" // Библиотека меню
#include "twi.h" // Библиотека для работы с шиной I2C (TWI)
#include "DS1307.h" // Библиотека для работы с часами реального времени (DS1307)
#include "Tachometr.h" // Библиотека на тахометр
#include "MC14489.h" // Библиотека для работы с драйвером 7-сегментных индикаторов MC14489DW
#include "adc.h" // Библиотека АЦП
#include "1wire.h" // Библиотека для работы с шиной 1-wire
#include "DS18B20.h" // Библиотека для работы с датчиком температуры (DS18B20)
#include "PWM.h" // Библиотека ШИМ
#include "BD.h" // База данных для термометра
#include "eeprom.h" // Библиотека для работы с EEPROM
#include "eepromext.h" // Библиотека для работы с внешней EEPROM памятью (24C64)
//===================================================

//========Вспомогательные макросы для работы с AVR========
// Вспомогательные общие макросы
#define _CONCAT_(x,y)		x ## y
// Вспомогательный макрос слияния символов
#define CONCAT(y,x)		_CONCAT_(y,x)

#define DDR(x)				CONCAT(DDR,x)
#define PORT(x)				CONCAT(PORT,x)
#define PIN(x)				CONCAT(PIN,x)

// Проверяем состояние пина PIN (bit) порта PINx (reg)
// Например: BitIsClear(PIN(PORT_stol1), PIN_stol1), где PIN(PORT_stolb1)=PINC, PIN_stolb1=5
#define BitIsClear(reg, bit)    ((reg & (1<<(bit))) == 0) // Если в регистре reg бита bit присутствует "0" 
#define BitRead(reg, bit)       (reg & (1<<(bit)))>>bit // Состояние бита порта (0 или 1)

/** Макрос для определения функции-инициализатора (вызывается автоматически в секции .inix)
  Х - номер секции от 0 до 9. Это секция для кода. Чтобы было понятно: сразу после сброса выполняется код, 
  помещенный в секцию .init0, потом постепенно номер секции увеличивается, и функция main выполняется в секции 10. 
  То есть пометив атрибутом с указанием определенной секции свою функцию, вы заставите ее выполниться ДО НАЧАЛА main. 
  Только будьте осторожны: параметры стека задаются в секции 2, а до этого стек еще не определен. Я рекомендую использовать 
  секции поближе к main, т.е. 6...9.
*/
#define INIT(x)			static void __attribute__((naked, used, section(".init" #x))) CONCAT(init, __COUNTER__) (void)
//========================================================

#endif /* MAIN_H_ */