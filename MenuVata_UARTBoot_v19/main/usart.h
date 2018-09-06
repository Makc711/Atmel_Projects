#ifndef USART_H_
#define USART_H_

#include "main.h"

// Исли используется Bootloader:
#define Boot_Address 0xFC00 // Адрес начала Bootloader'а для ATmega128

#define BAUD 19200 // Скорость обмена по USART, бод

// Если прием данных по USART не требуется, эту опцию надо установить в единичку
#define ONLY_OUT 0

void USART_Transmit_byte(uint8_t data); // Отправить 1 байт данных по USART
void USART_Transmit_word(uint16_t data); // Отправить слово (2 байта) по USART
void USART_Transmit_dword(uint32_t data); // Отправить двойное слово (4 байта) по USART
void USART_Transmit_msg(uint8_t *msg, uint16_t msgSize); // Отправить сообщение размером msgSize байт по USART
void USART_Transmit_string(const char *s); // Отправить строку данных по USART в кодировке ASCII

#endif /* USART_H_ */