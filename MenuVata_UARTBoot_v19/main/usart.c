#include "usart.h"
//----------------------------------------
uint8_t byteRX; // �������� ���� �� USART
char bf[20];
//----------------------------------------
INIT(7){ // ������������� USART
	#include <util/setbaud.h> // ���������� ��� ������� �������� UBRR_VALUE
	// ������� �������� ������ USART (ubrr)
	UBRR0H = UBRRH_VALUE; // �������� ������� ����� ����� ubrr � ������� UBRRH
	UBRR0L = UBRRL_VALUE; // �������� ������� ����� ����� ubrr � ������� UBRRL

	#if USE_2X // ���� ��������� �������� �������� ��� ���������� ������ USART
	UCSR0A |= (1<<U2X); // �������� �������� ������ �� USART (������������ ���� ����� ��������� �� ��������� �������, ����� ��������)
	#else
	UCSR0A &= ~(1<<U2X);
	#endif
	UCSR0B |= (1<<TXEN); // �������� �������� ������ �� USART
	#if ONLY_OUT == 0
	UCSR0B |= (1<<RXEN)|(1<<RXCIE); // �������� ����� ������ �� USART � ��������� ���������� ��� ��������� ������ ����� ������
	#endif
	
	UCSR0C |= (1<<UCSZ1)|(1<<UCSZ0); // 8-��� ������� (UCSZ1=1 � UCSZ0=1)
	//		     |(1<<USBS0); // 2 ����-���� (USBS0=1), ��� �������� �������� (UPM1=0 � UPM0=0), ������������ ����� (UMSEL=0)
}
//----------------------------------------
void USART_Transmit_byte(uint8_t data){ // ��������� 1 ���� ������ �� USART
	while ( !(UCSR0A & (1<<UDRE0)) ); // �������� ����������� �������� ������ �� ��������
	UDR0 = data; // ������ �������� ������
}
//----------------------------------------
void USART_Transmit_word(uint16_t data){ // ��������� ����� (2 �����) �� USART
	USART_Transmit_byte(data>>8); // ��������� ������� ���� ����� �� USART
	USART_Transmit_byte((uint8_t) data); // ��������� ������� ���� ����� �� USART
}
//----------------------------------------
void USART_Transmit_dword(uint32_t data){ // ��������� ������� ����� (4 �����) �� USART
	USART_Transmit_byte(data>>24); // ��������� ����� ������� ���� ����� �� USART
	USART_Transmit_byte(data>>16); // ��������� ���� ����� �������� �� USART
	USART_Transmit_byte(data>>8); // ��������� ���� ����� ��� ������ �� USART
	USART_Transmit_byte((uint8_t) data); // ��������� ����� ������� ���� ����� �� USART
}
//----------------------------------------
void USART_Transmit_msg(uint8_t *msg, uint16_t msgSize){ // ��������� ��������� �������� msgSize ���� �� USART
	for (uint16_t i=0; i<msgSize; i++){
		USART_Transmit_byte(msg[i]); // ���������� ����� �������
	}
}
//----------------------------------------
void USART_Transmit_string(const char *s){ // ��������� ������ ������ �� USART
	while (*s) USART_Transmit_byte(*s++); // ���� ������� ������ �� ����������, ���������� �� �� USART
}
//----------------------------------------
void (*Goto_Boot)(void); // ������� ������� Bootloader'�
//----------------------------------------
//___________������� ����������___________
//========================================
ISR(USART0_RX_vect){ // ���������� ������ ������ �� USART
	byteRX = UDR0;
//----------------------------------------
//____________����� Bootloader'�__________
//----------------------------------------
	if (byteRX == 0x7F){ // ���� ������ ��� 0x7F (��� ������� 'DEL' ��������� ASCII)
//##### ����� ��� ��� ���������� � Bootloader, ���� �������� ��� ������ �� ������
		cli(); // ��������� ���������� ����������
		hcms_puts("���������� ��"); // ����� ������ �� �������
		WriteStringMC14489("UPd", H_Alloff); // ��������� ������ ������ � �����
		LED_TEN_OFF; // ��������� ��������� ���
		LED_START_OFF; // ��������� ��������� ����
		TIMSK &= ~(1<<TOIE2); // ��������� ���������� ��� ������������ �������-�������� 2
		OCR1B = 0; // ���������� ��� (�����)
		PORT(PORT_PWMTEN) |= _BV(PIN_PWMTEN); // ������ 1 (����� ���)
//#####
		Goto_Boot = (void(*)())Boot_Address;  // ������������� ���������
		Goto_Boot();  // ������� �� ������ ������ Bootloader'�
	}
//----------------------------------------
	
}
//========================================