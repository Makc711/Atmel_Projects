#ifndef USART_H_
#define USART_H_

#include "main.h"

// ���� ������������ Bootloader:
#define Boot_Address 0xFC00 // ����� ������ Bootloader'� ��� ATmega128

#define BAUD 19200 // �������� ������ �� USART, ���

// ���� ����� ������ �� USART �� ���������, ��� ����� ���� ���������� � ��������
#define ONLY_OUT 0

void USART_Transmit_byte(uint8_t data); // ��������� 1 ���� ������ �� USART
void USART_Transmit_word(uint16_t data); // ��������� ����� (2 �����) �� USART
void USART_Transmit_dword(uint32_t data); // ��������� ������� ����� (4 �����) �� USART
void USART_Transmit_msg(uint8_t *msg, uint16_t msgSize); // ��������� ��������� �������� msgSize ���� �� USART
void USART_Transmit_string(const char *s); // ��������� ������ ������ �� USART � ��������� ASCII

#endif /* USART_H_ */