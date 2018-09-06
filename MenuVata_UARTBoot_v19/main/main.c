#include "main.h"
//===============���������� ����������===============
uint8_t mday = 1, // ���� ������ [1;31]
		month = 1, // ����� [1;12]
		year = 17, // ��� [0;99]
		wday = 1, // ���� ������ [1;7]
		hour = 12, // ��� [0;24]
		minute = 0, // ������ [0;59]
		second = 0; // ������� [0;59]
uint8_t DS1307_Read_flag = FALSE; // ���� ��� ������ ������� �� DS1307 (FALSE - �� ������; TRUE - ������)
uint16_t temper_set = 6000; // ������������� �����������, [15000;25000] *100'C
uint16_t rpm_set = 400, // ������������� ������� ��������, ��/��� [200;2000]
		 rpm_slow = 300, // ������� ���������� �������� (�� �������)
		 rpm_out = 0; // ������� ��������, ������������ �� ���������, ��/��� [200;2000]
uint16_t temper_realADC = 0; // ���������� �����������, ��� [0;1023]
uint16_t temper_realgrad = 0; // ���������� �����������, [2271;65535] *100'C
uint16_t voltage_ADC = 0; // ���������� ����, ��� [0;1023]
uint16_t current_ADC = 0; // ����� ���, ��� [0;1023]
uint8_t voltage = 0; // ���������� ����, �
uint8_t current_total = 0; // ����� ���, *10�
uint16_t power_cons = 0; // ������ ��������, ���*�
uint8_t change = FALSE; // ���� ��� ��������� �����������/RPM �� 7-��� ���������� (FALSE - ����./ TRUE - RPM)
extern uint8_t button_state; // ��� ������� ������
//------���������� ��� ���������------
extern uint8_t t2_1; // ���������� ������� ����� �������-�������� 2
extern uint16_t t2_cycles; // ���������� ���������� �������-�������� 2 c ������� ����������� �������� ���������� �� ������������ ������
extern uint32_t t2_ticks_int; // ���������� ����� �������-�������� 2 �� ������ ������
extern uint8_t flag_int; // ���� ��� ����������
extern uint16_t rpm; // ���������� ������� ��������, ��/��� (���������� ��������)
extern float rpmX; // ����������� �������� ���������� ������� ��������, ��/���
extern uint16_t get_rpm; // ���������� ������� ��������, ��/���
//-------���������� ��� ������--------
extern uint8_t pwm_mot; // ������� ��� ��� ������ [0;255]
//------------------------------------
uint16_t rpm_lcd = 0; // ������� ��������, ����������� �� �����, ��/���
uint8_t temper_lcd = 0; // �����������, ����������� �� ������, 'C
char bufled[4]; // ����� ��� ������ ���������� �� 7-���������� ���������� (4 �������)
uint8_t seccnt = 0; // ������� - ����� ��� ������� ":" � �����
uint8_t secstatus = FALSE; // ��������� ":" � ����� (FALSE - �������/TRUE - �����)
uint8_t start = FALSE; // ���� ������� ������ (FALSE - ����/TRUE - �����)
//------------------------------------
uint16_t temper_realDS = 0; // ���������� ��� �������� ������� ����������� [00000;12500] *100'C
extern uint8_t pwm_ten; // ������� ��� [0;100] (���)
//----------���������� ��� ��� (�����������)---------
int16_t e = 0; // ������� �������� ������, *100'C
extern float ef; // ������� �������� ������, 'C
extern float integral; // ������������ ������������
extern float e1; // ������ ���� ������ (������� ������)
//-----------���������� ��� ������� ������-----------
uint8_t secdata = 0; // ������� ������ ����� ������������� ����������� �� ������� EEPROM
uint16_t addrdata = 0; // ����� ������ ������, � ������� ������������ ���������
uint8_t writeDataEEPext = FALSE; // ���� ��� ������ ��������� �� ������� EEPROM (FALSE - �� �����; TRUE - �����)
uint8_t dataEEPext[16]; // ���������, ������������ �� ������� EEPROM
uint8_t twEEPextState = TWI_SUCCESS; // ������ ������ TWI
uint8_t errorEEPext = FALSE; // ���� �����������, ���� ������� EEPROM ������ ���������� (������� ����� ������ ������)
uint16_t freememory = FREE_EEPEXT; // ��������� EEPROM ������ *10, %
uint8_t addrEEPint = ADDREEP_BAD_EEPEXT_FIRST; // ����� ����� ������ ���������� EEPROM, � ������� ������������ ����� ������ ������ ������� EEPROM ��� ����������� ������
//-------���������� ��� ������ ���� "���������"------
uint8_t brightness_seg = 50; // ������� 7-����������� ����������, %
uint8_t brightness_led = 50; // ������� LED-����������, % (��� ��������� 2 %)
uint8_t reference_I = 100; // ���������� ��� ��������������� �����������, *100�
uint8_t reference_U = 220; // ���������� ���������� ��������������� �����������, �
uint16_t reference_IADC = 150; // ���������� �������� I ��� ��� reference_I, ���
uint16_t reference_UADC = 900; // ���������� �������� U ��� ��� reference_U, ���

extern uint8_t but_press; // ���� ��� ������������� "�������" �� ������ ��� �� ��������� (0-��� ������������� �������, ����� - ����)
uint8_t flag_set_led = FALSE; // ���� ��� ��������� 7-��� ���������� (TRUE - ������������� ��������; FALSE - ���������� ��������)
//===================================================

//-----------------------------------------------------
INIT(7){ 
	// ������������� ������� ���������� ��� �����:
    EICRA |= (1<<ISC21); // ������� ���������� INT2 �� ����������� ������
    EIMSK |= (1<<INT2); // �������� ������� ���������� INT2
	// ������������� ����������� ���, ����:
	DDR(PORT_LED) |= _BV(PIN_LED_TEN) | _BV(PIN_LED_START); // ���� ����� PORT_LED �� �����
}
//-----------------------------------------------------
ISR(INT2_vect){ // ������� ���������� INT2
	DS1307_Read_flag = TRUE; // ������ ����� � ���������� mday..second
	seccnt = 0; // �������� ������� (������� ":" � �����)
	secstatus = FALSE; // ����� ":" � �����
	power_cons += voltage * current_total / 1000; // ������������� ����������� �������� (���*�)
	if (start){ // ���� ���� ������� ����� (���� ������)
		if (secdata == 0){
			dataEEPext[0] = 0;
			dataEEPext[1] = mday; // ���� ������ [1;31]
			dataEEPext[2] = month; // ����� [1;12]
			dataEEPext[3] = year; // ��� [0;99]
			dataEEPext[4] = wday; // ���� ������ [1;7]
			dataEEPext[5] = hour; // ��� [0;24]
			dataEEPext[6] = minute; // ������ [0;59]
			dataEEPext[7] = second; // ������� [0;59]
			dataEEPext[8] = temper_set/100; // ������������� �����������, [150;250] 'C
			dataEEPext[9] = rpm_set/100; // ������������� ������� ��������, [2;20] /100��/��� 
			dataEEPext[10] = temper_lcd; // �����������, ����������� �� ������, [0;255] 'C
			dataEEPext[11] = rpm_lcd/100; // ������� ��������, ����������� �� �����, [0;20] /100��/���
			dataEEPext[12] = temper_realDS/100; // ������� �����������, [0;125] 'C
			dataEEPext[13] = voltage; // ���������� ����, [0;255] �
			dataEEPext[14] = current_total; // ����� ���, [0;255] *10�
			dataEEPext[15] = power_cons; // ������ ��������, [0;255] ���*�
//			writeDataEEPext = TRUE; // ��������� ����
			secdata = INTERVAL_BW_MSG; // �������� 30 ������ ����� �������� ������
		}
		else secdata--; // ������� ������ ��� �������� ��������� ����� ������������� ������� �� ������� EEPROM
	}
}
//-----------------------------------------------------
int main(void){
	sei(); // ��������� ���������� ����������
	
	DS1307_Read_Time(); // ��������� ����� � ���������� mday..second (��� ������)
	
	if (EEPROM_read_word(ADDREEP_TEMPER_SET) != EEP_CLEN_WORD) // ���� EEPROM ������ �� ������
		temper_set = EEPROM_read_word(ADDREEP_TEMPER_SET); // ��������� ������������� ����������� �������
	if (EEPROM_read_word(ADDREEP_RPM_SET) != EEP_CLEN_WORD)
		rpm_set = EEPROM_read_word(ADDREEP_RPM_SET); // ��������� ������������� ������� ��������
	if (EEPROM_read_byte(ADDREEP_BR_LED) != EEP_CLEN_BYTE)
		brightness_led = EEPROM_read_byte(ADDREEP_BR_LED); // ��������� ������������� ������� LED-����������
	if (EEPROM_read_byte(ADDREEP_REF_U) != EEP_CLEN_BYTE)
		reference_U = EEPROM_read_byte(ADDREEP_REF_U); // ��������� ��������������� �������� U
	if (EEPROM_read_word(ADDREEP_REF_UADC) != EEP_CLEN_WORD)
		reference_UADC = EEPROM_read_word(ADDREEP_REF_UADC); // ��������� ��������������� �������� UADC
	if (EEPROM_read_byte(ADDREEP_REF_I) != EEP_CLEN_BYTE)
		reference_I = EEPROM_read_byte(ADDREEP_REF_I); // ��������� ��������������� �������� I
	if (EEPROM_read_word(ADDREEP_REF_IADC) != EEP_CLEN_WORD)
		reference_IADC = EEPROM_read_word(ADDREEP_REF_IADC); // ��������� ��������������� �������� IADC
	
	while(TRUE){
		MenuProcess(); // ����� ������� ����
		
		if (DS1307_Read_flag){ // ���� ���� ������
			DS1307_Read_Time(); // ��������� ����� � ���������� mday..second
			DS1307_Read_flag = FALSE; // �������� ����
		}
		
		temper_realDS = dt_converttemp(dt_check()); // �������� ������� ����������� [00000;12500] *100'C
		current_ADC = adc_data(ADC_CH_0); // ������ �������� ��� (���)
		temper_realADC = adc_data(ADC_CH_1); // ������ �������� ��� (�����������)
		voltage_ADC = adc_data(ADC_CH_2); // ������ �������� ��� (����������)
		temper_realgrad = pgm_read_word(&table_termores[temper_realADC]); // ��������� ����������� 'C �� ����������� �������� ���
		//------------���--------------------------
		e = temper_set - temper_realgrad; // ������ (�������������-���������� �����������)
		if (abs(e) > INTEGRAL_GIST_GRAD) // ���� �������� ���������� ������ 2 'C
			integral = 0; // �������� ��������
		ef = (float) e / 100; // ��������� *100'C � 'C
		//-----------------------------------------
		if (start){ // ���� ���� ������� ����� (���� ������)
			if (change){ // ���� ������������ ��������� �����������
				if (button_state == Butt_Up){ // ���� ��������� ������� ������ �����
					but_press = TRUE; // ��������� ������������ "�������" �� ������
					flag_set_led = TRUE; // ������� �� 7-��� ��������� ������������� ��������
					if (temper_set < TEMPER_SETMAX) // ����������� �� ������������ ����������� �������
						temper_set += 100; // ����������� ����������� �������
				}
				else if (button_state == Butt_Down){ // ���� ��������� ������� ������ ����
					but_press = TRUE; // ��������� ������������ "�������" �� ������
					flag_set_led = TRUE; // ������� �� 7-��� ��������� ������������� ��������
					if (temper_set > TEMPER_SETMIN) // ����������� �� ����������� ����������� �������
						temper_set -= 100; // ��������� ����������� �������
				}
			}
			else { // ���� ������������ ��������� ������� ��������
				if (button_state == Butt_Up){ // ���� ��������� ������� ������ �����
					but_press = TRUE; // ��������� ������������ "�������" �� ������
					flag_set_led = TRUE; // ������� �� 7-��� ��������� ������������� ��������
					if (rpm_set < RPM_SETMAX) // ����������� �� ������������ ����������� �������
						temper_set += 100; // ����������� ����������� �������
				}
				else if (button_state == Butt_Down){ // ���� ��������� ������� ������ ����
					but_press = TRUE; // ��������� ������������ "�������" �� ������
					flag_set_led = TRUE; // ������� �� 7-��� ��������� ������������� ��������
					if (rpm_set > RPM_SETMIN) // ����������� �� ����������� ����������� �������
						temper_set -= 100; // ��������� ����������� �������
				}
			}
			// ���� ����������� �� �������� �� ������ ��������� ������, ������ ��������
			if (temper_realgrad < temper_set - RPM_GIST_GRAD) // ���� ���� �� ��������� �� ������ �����������
				rpm_out = rpm_slow; // ������ ��������
			else 
				rpm_out = rpm_set; // ������ ���������
		}
		//-----------------------------------------
		current_total = current_ADC * reference_I / reference_IADC; // ����� ���, *100� | *���� (���������� ���, � *100 / current_ADC) (1 � ��� 150 ���)
		voltage = (uint32_t) voltage_ADC * reference_U / reference_UADC; // ���������� ����, � | *���� (���������� ����������, � / voltage_ADC) (220 � ��� 900 ���)
		
		if (change){ // ���� ���������� ����
			if (flag_set_led){ // ���� ����� ������� ������������� ��������
				sprintf(bufled,"%4u",rpm_set); // ������� RPM
				WriteStringMC14489(bufled, H_Alloff); // ��������� ������ ������ � �����
			}
			else{ // ���� ����� ������� ���������� ��������
				rpm_lcd = (get_rpm + 50)/100*100; // ��������� �� �����
				sprintf(bufled,"%4u",rpm_lcd); // ������� RPM
				WriteStringMC14489(bufled, H_Alloff); // ��������� ������ ������ � �����
			}
		}
		else {
			if (flag_set_led){ // ���� ����� ������� ������������� ��������
				sprintf(bufled,"%3uC",temper_set/100); // ������� �����������
				WriteStringMC14489(bufled, H_BANK4); // ��������� ������ ������ � �����
			}
			else{ // ���� ����� ������� ���������� ��������
				if (temper_realgrad < temper_realDS + 1000) // ���� ����������� ������� ������ ������� ����������� �� 10 'C
					temper_lcd = (temper_realDS + 50)/100; // ��������� �� ������
				else
					temper_lcd = (temper_realgrad + 50)/100; // ��������� �� ������
				sprintf(bufled,"%3uC",temper_lcd); // ������� �����������
				WriteStringMC14489(bufled, H_BANK4); // ��������� ������ ������ � �����
			}
		}

		if (button_state == Butt_Start){ // ���� ��������� ������� ������ Start
			if (!start){ // ���� �� ����� ���� ������� ����
				LED_START_ON; // �������� ��������� ����
				TIMSK |= (1<<TOIE2); // ��������� ���������� ��� ������������ �������-�������� 2
				EIMSK |= (1<<INT4); // ��������� ������� ���������� INT4
				pwm_mot = PWMSTA; // ������ ��������� �������� ��� ��� �������� ������ ������
				start = TRUE; // ��������� ����
			}
		}
		else if (button_state == Butt_Stop){ // ���� ��������� ������� ������ Stop
			if (start){ // ���� �� ����� ���� ������� �����
				//____��������� ����������___
				LED_TEN_OFF; // ��������� ��������� ���
				LED_START_OFF; // ��������� ��������� ����
				//_____�������� ��������_____
				TIMSK &= ~(1<<TOIE2); // ��������� ���������� ��� ������������ �������-�������� 2
				EIMSK &= ~(1<<INT4); // ��������� ������� ���������� INT4
				t2_1 = 0; // �������� ���������� ������� ����� �������-�������� 2
				t2_cycles = 0; // ������� ������� ���������� �������-�������� 2
				t2_ticks_int = 0; // ������� ������� ����� �������-�������� 2 �� ������ ������
				flag_int = TRUE; // ��������� ���� ��� ����������
				rpm = 0; // ������� ���������� ������� ��������
				rpmX = 0; // ������� ����������� �������� ������� ��������
				get_rpm = 0; // ������� �������� �������� ������� ��������
				//_____������������� �����____
				rpm_out = 0; // ������������� �����
				pwm_mot = 0; // �������� ������������� �������� ���
				OCR1B = 0; // ���������� ��� (�����)
				//_______��������� ���________
				pwm_ten = 0; // ������������� ��� � 0
				PORT(PORT_PWMTEN) |= _BV(PIN_PWMTEN); // ������ 1 (����� ���)
				integral = 0; // �������� ��������
				e1 = 0; // �������� ������
				//____________________________
				secdata = 0; // �������� �������, ���� ���� ������� ����
				//____________________________
				start = FALSE; // ������ ����
			}
		}
		else if (button_state == Butt_Change){ // ���� ��������� ������� ������ Change
			change = !change; // ���� ��� ����� ��������� �����������/RPM �� 7-��� ����������
		}
		
		if (writeDataEEPext){ // ���� ���� ������
			twEEPextState = EE_Write_msg(addrdata, dataEEPext, BYTES_TO_EEPEXT); // �������� ��������� �� ������� EEPROM �� ������
			if (twEEPextState != TWI_SUCCESS){ // ���� �� ������� �������� ������ �� ����� ������
				EEPROM_write_word(addrEEPint, addrdata); // �������� ���� ����� � EEPROM
				if (addrEEPint < ADDREEP_BAD_EEPEXT_LAST) // ����������� �� ������������ ������
					addrEEPint += BAD_EEPEXT_BYTE; // ��������� �� ��������� �����
				else errorEEPext = TRUE; // ������� EEPROM ������ ����������
			}
			if (addrdata < (MEMORY-BYTES_TO_EEPEXT)){ // ����������� �� ������������ ������
				addrdata += BYTES_TO_EEPEXT; // ��������� �� ��������� �����
				freememory = FREE_EEPEXT - ((addrdata / (MEMORY-BYTES_TO_EEPEXT)) * FREE_EEPEXT); // ��������� ��������� ������ *10, %
			}
			writeDataEEPext = FALSE; // �������� ����
		}
	}
}
