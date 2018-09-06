#ifndef keyboardXY_H_
#define keyboardXY_H_

#include "main.h"

//-------------------------------------//
//        ��� ������� ������           //
//-------------------------------------//
enum {
	Butt_Esc = 1,
	Butt_Left,
	Butt_Down,
	Butt_Up,
	Butt_Right,
	Butt_Select,
	Butt_Start,
	Butt_Change,
	Butt_Stop
};
//--------------------------------------

#define key_null 0x00     // ������ �� ������
#define key11 Butt_Esc    // ������ ������ ESC
#define key12 Butt_Left   // ������ ������ �����
#define key13 Butt_Down   // ������ ������ ����
#define key21 Butt_Up     // ������ ������ �����
#define key22 Butt_Right  // ������ ������ ������
#define key23 Butt_Select // ������ ������ SELECT
#define key31 Butt_Stop   // ������ ������ STOP
#define key32 Butt_Change // ������ ������ T/RPM
#define key33 Butt_Start  // ������ ������ START

// ���� ��� ����� ���������� �� 1 ����� - ����� ������� ���,
// ����� ������ ����������������!
#define 	PORT_KEYBOARD		C

// ����� ���� �������� ������ � ����� ��������� ����������.
// ����� ���� - ��� ����� ���� � �����, �.�. ����� �� 0 �� 7
#define		PORT_stolb1			PORT_KEYBOARD // ���� ������� 1
#define		PIN_stolb1			5             // ��� ������� 1

#define		PORT_stolb2			PORT_KEYBOARD // ���� ������� 2
#define		PIN_stolb2			4             // ��� ������� 2

#define		PORT_stolb3			PORT_KEYBOARD // ���� ������� 3
#define		PIN_stolb3			3             // ��� ������� 3

#define		PORT_stroka1		PORT_KEYBOARD // ���� ������ 1
#define		PIN_stroka1			2             // ��� ������ 1

#define		PORT_stroka2		PORT_KEYBOARD // ���� ������ 2
#define		PIN_stroka2			1             // ��� ������ 2

#define		PORT_stroka3		PORT_KEYBOARD // ���� ������ 3
#define		PIN_stroka3			0             // ��� ������ 3

#define THRESHOLD 20 // ������� ������ ������ ������ ������ ������ ������������ (������ �� ��������)
#define TIMEPRESS_SLOW 150 // ����� ����� "���������� ���������" �� ������ ��� �� ��������� [TIMEPRESS_FAST;255-THRESHOLD)
#define TIMEPRESS_FAST 50 // ����� ����� "�������� ���������" �� ������ ��� �� ��������� (10;TIMEPRESS_SLOW]
#define CLICKS 3 // ���������� "��������� �������" �� ������ ����� "�������� ���������"

#define PRESCALER_T0 64 // ������������ �������-�������� 0
#define TICK_INT_T0 256 // ���������� ����� �0 �� ���������� �� ������������ (2^8 ��� 2^16)
#define TICKSEC (F_CPU/PRESCALER_T0/TICK_INT_T0) // ���������� ����� �0 �������� �� 1 ��� (������ ���������� �0)
#define HALFSEC (TICKSEC/2) // ���������� ����� �0 �������� �� 500 ��
#define TICK750 ((TICKSEC*75/100)+1) // ���������� ����� �0 �������� �� 750 �� (+1 - ��� ���� ����� ���� �� ������ 750 ��)
#define TICK0_WRITE_CYCLE ((TICKSEC*WRITE_CYCLE/1000) + 2) // ���������� ����� �0 �� ������������ �������� �� 10 �� (450*10/1000+1 = 5) (+2 - ��� ���� ����� ���� �� ������ 10 ��)

uint8_t KEYB_GetKey(void);       // ����� ��� ������� ������

#endif //keyboardXY_H_