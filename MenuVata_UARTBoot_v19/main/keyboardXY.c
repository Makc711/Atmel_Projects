#include "keyboardXY.h"
//-----------------------------------------
volatile uint8_t pressedKey = key_null; // ��������� �������� ���� ������� ������
uint8_t comp=0; // ������������ ��� ������ �� ��������
uint8_t but_press = FALSE; // ���� ��� ������������� "�������" �� ������ ��� �� ���������
uint8_t time_press = 5; // ����� ����� "���������" �� ������ ��� �� ���������
uint8_t time_press_long = TIMEPRESS_SLOW; // ����� ����� "���������" �� ������ ��� �� ���������
uint8_t click = 0; // ������� "��������� �������" �� ������
//-----------------------------------------
uint16_t tim_cnt=0;// ������� ����� ������� (����� ��� ������� �������� ����)
extern uint8_t seccnt; // ������� - ����� ��� ������� ":" � �����
extern uint8_t secstatus; // ��������� ":" � ����� (FALSE - �������/TRUE - �����)
// ���������� ��� �������� DS18B20
extern uint16_t tim_cntDS; // ������� ����� ��� �������� DS18B20
extern uint8_t startDS; // ���� ������ ��������������
extern uint8_t stopDS; // ���� ���������� ��������������
// ���������� ��� �������� twi
extern uint8_t writeTime; // ������ (����� ��� ������� ������� ����� ������� ������ eepromext)

extern uint8_t flag_set_led; // ���� ��� ��������� 7-��� ���������� (TRUE - ������������� ��������; FALSE - ���������� ��������)
uint16_t set_led_cnt = 0; // ������� ��� �������� ������� ��� ��������� �������������� �������� �� 7-���
//-----------------------------------------

void init_button_timer(void){
	TCCR0 |= (1<<CS02)|(0<<CS01)|(0<<CS00); // ������������� ������������ 64
	// ��� ����� �������� - ������� ������������ (7372800) / ������������ / 256 = 450 (����� 2-� ����������)
	// ����� �� 256, ��� ��� ������ ������� ������ ������� �� 0x00 �� 0xFF � ��� �� ������������ �����
	TIMSK |= (1<<TOIE0); // ������������� ��� ���������� ���������� 0-��� �������� �� ������������
}
//----------------------------------------
INIT(7){ // ������������� ����������
#if defined(PORT_KEYBOARD)
	DDR(PORT_KEYBOARD) &= ~(_BV(PIN_stolb1) | _BV(PIN_stolb2) | _BV(PIN_stolb3) | _BV(PIN_stroka1) | _BV(PIN_stroka2)); // ����� ����� - �����
	PORT(PORT_KEYBOARD) |= _BV(PIN_stolb1) | _BV(PIN_stolb2) | _BV(PIN_stolb3) | _BV(PIN_stroka1) | _BV(PIN_stroka2); // � �������������� �����������
#else
 	DDR(PORT_stolb1) &= ~_BV(PIN_stolb1); // ������� - �����
 	PORT(PORT_stolb1) |= _BV(PIN_stolb1); // ������� - ����� � �������������� �����������
 	DDR(PORT_stolb2) &= ~_BV(PIN_stolb2); // ������� - �����
 	PORT(PORT_stolb2) |= _BV(PIN_stolb2); // ������� - ����� � �������������� �����������
 	DDR(PORT_stolb3) &= ~_BV(PIN_stolb3); // ������� - �����
 	PORT(PORT_stolb3) |= _BV(PIN_stolb3); // ������� - ����� � �������������� �����������
	DDR(PORT_stroka1) &= ~_BV(PIN_stroka1); // ������ - ���� �����
	PORT(PORT_stroka1) |= _BV(PIN_stroka1); // ������ - ���� ����� � �������������� �����������
	DDR(PORT_stroka2) &= ~_BV(PIN_stroka2); // ������ - ���� �����
	PORT(PORT_stroka2) |= _BV(PIN_stroka2); // ������ - ���� ����� � �������������� �����������
#endif
// ����������: _BV(3) ���������� ������� (1<<3) (����������� �����)
// ��������: PORTD|=_BV(3); == PORTD|=(1<<3); => 0bxxxx1xxx // �������� ���������� ������������� �������� � ����� 3 ����� D
//			 DDRD&=~_BV(3); == DDRD&=~(1<<3); // ��������� ����� 3 ����� D �� ����
	
	init_button_timer(); // ������������� �������
}

 void KEYB_ScanKeyboard (void){
	uint8_t key = 0;
	
// ������������ ����������� ������ �� ��	
 	DDR(PORT_stroka1) |= _BV(PIN_stroka1); // ������ ������ 1 �������
 	PORT(PORT_stroka1) &= ~_BV(PIN_stroka1); _delay_us(100); // � ��� � �� 0
		 if (BitIsClear(PIN(PORT_stolb1), PIN_stolb1)) key = key11; // ���� �� � 1-� �������, �� ������ ������� 11
	else if (BitIsClear(PIN(PORT_stolb2), PIN_stolb2)) key = key12; // ���� �� ����� PORT_stolb2 ����� PIN(PORT_stolb2) ������������ "0" 
	else if (BitIsClear(PIN(PORT_stolb3), PIN_stolb3)) key = key13;
	PORT(PORT_stroka1) |= _BV(PIN_stroka1); // � ������ 1 ���������� 1
	DDR(PORT_stroka1) &= ~_BV(PIN_stroka1); // � ������ � ������
	
	DDR(PORT_stroka2) |= _BV(PIN_stroka2);
	PORT(PORT_stroka2) &= ~_BV(PIN_stroka2); _delay_us(100);
		 if (BitIsClear(PIN(PORT_stolb1), PIN_stolb1)) key = key21;
	else if (BitIsClear(PIN(PORT_stolb2), PIN_stolb2)) key = key22;
	else if (BitIsClear(PIN(PORT_stolb3), PIN_stolb3)) key = key23;
	PORT(PORT_stroka2) |= _BV(PIN_stroka2);
	DDR(PORT_stroka2) &= ~_BV(PIN_stroka2);
		
	DDR(PORT_stroka3) |= _BV(PIN_stroka3);
	PORT(PORT_stroka3) &= ~_BV(PIN_stroka3); _delay_us(100);
		if (BitIsClear(PIN(PORT_stolb1), PIN_stolb1)) key = key31;
	else if (BitIsClear(PIN(PORT_stolb2), PIN_stolb2)) key = key32;
	else if (BitIsClear(PIN(PORT_stolb3), PIN_stolb3)) key = key33;
	PORT(PORT_stroka3) |= _BV(PIN_stroka3);
	DDR(PORT_stroka3) &= ~_BV(PIN_stroka3);	

	  if (key){ // ���� �� ��������� ���������� "key" ���-�� ����
		  if (but_press){ // ���� ���� ��� ������������� "�������" ������
			  if (click < CLICKS) time_press = TIMEPRESS_SLOW; // "��������� �������" �� ������
			  else time_press = TIMEPRESS_FAST; // "������� �������" �� ������
			  time_press_long = time_press;
		  }
		  else time_press = 5;
		  if (comp == THRESHOLD){ // � ���� ������ ������������ THRESHOLD ������ ������
			  comp = THRESHOLD+10; // ����� ��� ��������� ������ pressedKey �� �����������
			  pressedKey = key; // �������� �� ����� � �����
			  return;
		  }
		  else if (comp < (THRESHOLD + time_press)) // ���� ������ ������������ ������ (THRESHOLD+time_press) ������ ������ (������ �� ��������)
			  comp++; // (THRESHOLD+200) ������ ������ ������ - �������� 0,5 ���
		  else if (comp >= (THRESHOLD + time_press_long)){ // ���� ������ ������������ ������ (THRESHOLD+TIMEPRESS) ������ ������
			  comp=0; // ��������, ����� ���� ����������� �������� pressedKey
			  if (click < 255) click++; // ������� ���������� "��������� �������" �� ������
		  }
	  }
	  else {
		  but_press = FALSE; // ��������� ������������ "�������" �� ������
		  comp=0; // �������� ��� ���������� ������
		  click = 0; // �������� ������� "��������� �������" �� ������
	  }
}

// ���������� ��� ������� ������
uint8_t KEYB_GetKey(void){
	uint8_t key = pressedKey;
	pressedKey = key_null;
	return key;
}

ISR (TIMER0_OVF_vect){
//===============����������================
	KEYB_ScanKeyboard(); // ��������� ����������
//=============������� ����================
	if (tim_cnt++ == TICKSEC) // ���� ��������� �� 1 ���
		tim_cnt = 0; // ������� ����� ��� ������� �������� ����
//=======�������� 1 ��� ��� 7-���==========
	if (flag_set_led){ // ���� ���� �������� (���������� ��������)
		if (set_led_cnt++ == TICKSEC){ // ���� ��������� �� 1 ���
			set_led_cnt = 0; // ���������� �������
			flag_set_led = FALSE; // ������� ���� (������� ���������� ��������)
		}
	}
//=====������� ��� �������� DS18B20========
	if (startDS) // ���� �������������� ���� ������
		tim_cntDS++; // ������� ����
	if (tim_cntDS == TICK750){ // ���� ������ 750 ��
		startDS = FALSE; // ������� ���� (������������� �������, ����� ��������� ��������������)
		tim_cntDS = 0; // ���������� �������
		stopDS = TRUE; // ��������� ���� (����� ��������� ��������������)
	}
//=========������� ":" � �����=============
	if (seccnt < 255) 
		seccnt++; // ������� ����� ��� ������� ":" � ����� (����� ��������� �� 0,566 ���.)
	if (seccnt == HALFSEC) 
		secstatus = TRUE; // �������� ":" � �����
//=====������� ��� �������� twi========
	if (writeTime>0)
		writeTime--; // �������� ����� ������� ������ � eepromext
}
//-----------------------------------------