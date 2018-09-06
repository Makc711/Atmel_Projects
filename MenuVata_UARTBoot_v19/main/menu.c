#include "menu.h"
//-----------------------------------------------------------
//--------------���������� ���������� ������-----------------
void MainMenuProcess(void); // ������� �������� ����
void SubmenuClockProcess(void); // ������� ������� "����"
void SubmenuMemoryProcess(void); // ������� ������� "������:"
void SubmenuSettingsProcess(void); // ������� ������� "���������"
//-----------------------------------------------------------
char str_disp[16]; // ������ ��� ������ ������ �� �������
char str_prog16[16]; // ������ ��� �������� ����� �� flash �������� 16 ����
char str_prog3[3]; // ������ ��� �������� ����� �� flash �������� 3 �����
//-----------------------------------------------------------
uint8_t button_state = key_null; // ��� ������� ������
extern uint8_t but_press; // ���� ��� ������������� "�������" �� ������ ��� �� ��������� (0-��� ������������� �������, ����� - ����)
uint8_t direction = DIRECTION_NO; // ����������� �������� �� ����
//===============���������� ����������===============
extern uint16_t tim_cnt; // ������� ����� ������� (����� ��� ������� ���������� ������ ����)
extern uint8_t mday, // ���� ������ [1;31]
			   month, // ����� [1;12]
			   year, // ��� [0;99]
			   wday, // ���� ������ [1;7]
			   hour, // ��� [0;24]
			   minute, // ������ [0;59]
			   second; // ������� [0;59]
extern uint16_t temper_set; // ������������� �����������, '� [15000;25000]/100
extern uint16_t rpm_set; // ������������� ������� ��������, ��/���
extern uint8_t voltage; // ���������� ����, �
extern uint8_t current_total; // ����� ���, *10�
extern uint16_t power_cons; // ������ ��������, ���*�
//------���������� ��� ���������------
extern uint8_t t2_1; // ���������� ������� ����� �������-�������� 2
extern uint16_t t2_cycles; // ���������� ���������� �������-�������� 0 c ������� ����������� �������� ���������� �� ������������ ������
extern uint32_t t2_ticks_int; // ���������� ����� �������-�������� 0 �� ������ ������
extern uint8_t flag_int; // ���� ��� ����������
extern uint16_t rpm; // ���������� ������� ��������, ��/��� (���������� ��������)
extern float rpmX; // ����������� �������� ���������� ������� ��������, ��/���
extern uint16_t get_rpm; // ���������� ������� ��������, ��/���
//-------���������� ��� ������--------
extern uint8_t pwm_mot; // ������� ��� ��� ������ [0;255]
//------------------------------------
extern uint8_t seccnt; // ������� - ����� ��� ������� ":" � �����
extern uint8_t secstatus; // ��������� ":" � ����� (0 - �������/1 - �����)
//----------���������� ��� ��� (�����������)---------
extern uint8_t pwm_ten; // ������� ��� [0;100] (���)
extern float integral; // ������������ ������������
extern float e1; // ������ ���� ������ (������� ������)
extern uint8_t start; // ���� ������� ������ (0 - ����/1 - �����)
//-----------���������� ��� ������� ������-----------
extern uint16_t freememory; // ��������� EEPROM ������ *10, %
//-------���������� ��� ������ ���� "���������"------
extern uint8_t brightness_seg; // ������� 7-����������� ����������, %
extern uint8_t brightness_led; // ������� LED-����������, % (��� ��������� 2 %)
extern uint8_t reference_U; // ���������� ���������� ��������������� �����������, �
extern uint8_t reference_I; // ���������� ��� ��������������� �����������, *100�
extern uint16_t reference_UADC; // ���������� �������� U ��� ��� reference_U, ���
extern uint16_t reference_IADC; // ���������� �������� I ��� ��� reference_I, ���
extern uint16_t voltage_ADC; // ���������� ����, ��� [0;1023]
extern uint16_t current_ADC; // ����� ���, ��� [0;1023]
//===================================================
//--------------������ �������� ����-----------------
const char MMenuItem1[] PROGMEM = "������ ��: ";
const char MMenuItem2[] PROGMEM = "RPM: ";
//const char MMenuItem3[] PROGMEM = "RPM slow: ";
const char MMenuItem3[] PROGMEM = "������: ";
const char MMenuItem4[] PROGMEM = "���� � �����";
const char MMenuItem5[] PROGMEM = "����������: ";
const char MMenuItem6[] PROGMEM = "���: ";
//const char MMenuItem8[] PROGMEM = "����-��: ";
const char MMenuItem7[] PROGMEM = "������: "; // (Memory) ���������� �� SD; ��������? �� ���; 
const char MMenuItem8[] PROGMEM = "���������"; // (Settings) ������� 7-����.; ������� LED ���.; ���������� U; ���������� I; 
//------------������ ������� "������:"----------------
const char MemoryItem1[] PROGMEM = "���������� �� SD"; // ��������: 100%; ��� SD �����;
const char MemoryItem2[] PROGMEM = "�������� ������"; // ��������: 100%; ������ �������; Bad memory; 
const char MemoryItem3[] PROGMEM = "   ��     ���   ";
const char MemoryItem4[] PROGMEM = "��������: "; // (��������: 100%)
const char MemoryItem5[] PROGMEM = "  �����������   ";
const char MemoryItem6[] PROGMEM = "  ��� SD �����  ";
const char MemoryItem7[] PROGMEM = " ������ ������� ";
const char MemoryItem8[] PROGMEM = "   Bad memory   ";
//-----------������ ������� "���������"---------------
const char SettingsItem1[] PROGMEM = "������� 7-����.";
const char SettingsItem2[] PROGMEM = "������� LED ���.";
const char SettingsItem3[] PROGMEM = "���������� U";
const char SettingsItem4[] PROGMEM = "���������� I";
//-------------������������ ���� ������---------------
const char ItemDay1[] PROGMEM = "��";
const char ItemDay2[] PROGMEM = "��";
const char ItemDay3[] PROGMEM = "��";
const char ItemDay4[] PROGMEM = "��";
const char ItemDay5[] PROGMEM = "��";
const char ItemDay6[] PROGMEM = "��";
const char ItemDay7[] PROGMEM = "��";
PGM_P const string_tableDay[] PROGMEM = {
	ItemDay1, 
	ItemDay2, 
	ItemDay3, 
	ItemDay4, 
	ItemDay5, 
	ItemDay6, 
	ItemDay7
};
//-------------������������ �������---------------
const char ItemMonth1[]  PROGMEM = "���";
const char ItemMonth2[]  PROGMEM = "���";
const char ItemMonth3[]  PROGMEM = "���";
const char ItemMonth4[]  PROGMEM = "���";
const char ItemMonth5[]  PROGMEM = "���";
const char ItemMonth6[]  PROGMEM = "���";
const char ItemMonth7[]  PROGMEM = "���";
const char ItemMonth8[]  PROGMEM = "���";
const char ItemMonth9[]  PROGMEM = "���";
const char ItemMonth10[] PROGMEM = "���";
const char ItemMonth11[] PROGMEM = "���";
const char ItemMonth12[] PROGMEM = "���";
PGM_P const string_tableMonth[] PROGMEM = {
	ItemMonth1, 
	ItemMonth2, 
	ItemMonth3, 
	ItemMonth4, 
	ItemMonth5, 
	ItemMonth6, 
	ItemMonth7,
	ItemMonth8,
	ItemMonth9,
	ItemMonth10,
	ItemMonth11,
	ItemMonth12
};
//----------------������� ":" � �����------------------
char secsta[2][2]= // 2 ��������, 1 ������ + 1 �� \0 - ����� ������
{
	" \0",
	":\0",
};
//-----------------------------------------------------
// ������ ��������� ������ ����:
MENU_StateTypeDef menu_state = MENU_STATE_IDLE;
MAIN_MENU_StateTypeDef main_menu_state = MAIN_MENU_STATE_MM1_IDLE;
SUBMENU_CLOCK_StateTypeDef submenu_clock_state = SUBMENU_CLOCK_STATE_IDLE;
SUBMENU_MEMORY_StateTypeDef submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE;
SUBMENU_SETTINGS_StateTypeDef submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE;
//-----------------------------------------------------
void direction_str_disp(char *s){ // ������� ������ str_disp � ������������ ������ �� LED ���������
	switch (direction)
	{
		case DIRECTION_DOWN: // ���� ��������� ����
			direction = DIRECTION_NO; // ��� ����������� ��������
			hcms_rollower_puts_all(s, DOWN);
			break;
		case DIRECTION_UP: // ���� ��������� �����
			direction = DIRECTION_NO; // ��� ����������� ��������
			hcms_rollower_puts_all(s, UP);
			break;
		case DIRECTION_LEFT: // ���� ��������� �����
			direction = DIRECTION_NO; // ��� ����������� ��������
			hcms_shiftleft_puts(s); 
			break;
		case DIRECTION_RIGHT: // ���� ��������� ������
			direction = DIRECTION_NO; // ��� ����������� ��������
			hcms_shiftright_puts(s);
			break;
		default: // ���� ��� ����������� ��������
			hcms_puts(s); 
	}
}

void MenuProcess(void) // ������� ����
{
	switch (menu_state) // ������� �� ������ ����
	{
		case MENU_STATE_IDLE: // ����� ���������
			strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
			sprintf(str_disp," %2u%s%02u %s %2u-%02u",hour,secsta[secstatus],minute,str_prog3,mday,month); // ������ ������ � �����
			direction_str_disp(str_disp); // ����� ������ �� �������
			menu_state = MENU_STATE_WAIT; // ���������� ������: "���� ������� �������� ����"
			break;
		case MENU_STATE_WAIT: // ���� ������� �������� ���� (������� ������)
			strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
			sprintf(str_disp," %2u%s%02u %s %2u-%02u",hour,secsta[secstatus],minute,str_prog3,mday,month); // ������ ������ � �����
			hcms_puts(str_disp); // ����� ������ �� �������
			button_state = KEYB_GetKey(); // ���������� ��� ������� ������ � ����������
			if (button_state == Butt_Select){ // ���� ��������� ������� ������ Select
				direction = DIRECTION_RIGHT; // ��������� �������
				menu_state = MENU_STATE_MAIN; // ���������� ������: "������ �������� ����"
			}
			break;
		case MENU_STATE_MAIN: // ������ �������� ����
		//##### ��������� ������ ����������� � ������ #####
		//___����� ������ �� 7-���������� ����������
			WriteStringMC14489("----", H_Alloff); // ��������� ������ ������ � �����
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
			pwm_mot = 0; // �������� ������������� �������� ���
			OCR1B = 0; // ���������� ��� (�����)
		//_______��������� ���________
			pwm_ten = 0; // ������������� ��� � 0
			PORT(PORT_PWMTEN) |= _BV(PIN_PWMTEN); // ������ 1 (����� ���)
			integral = 0; // �������� ��������
			e1 = 0; // �������� ������
		//____________________________
			start = 0; // ������ ����
		//#################################################
			main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // ���������� ������ ����: "����� 1 �������� ����"
			MainMenuProcess(); // ����� ������� �������� ����
			break;
	}
}
//-----------------------------------------------------
void MainMenuProcess(void) // ������� �������� ����
{
uint16_t temper_set_tmp = temper_set; // ���������� ������� �������� ������������� ����������� �� ��������� ����������
uint16_t rpm_set_tmp = rpm_set; // ���������� ������� �������� ������������� ������� �������� �� ��������� ����������
	while (TRUE)
	{
		//-----------------------------------------
		current_ADC = adc_data(ADC_CH_0); // ������ �������� ��� (���)
		voltage_ADC = adc_data(ADC_CH_2); // ������ �������� ��� (����������)
		current_total = current_ADC * reference_I / reference_IADC; // ����� ���, *10� | *���� (���������� ���, � *10 / current_ADC) (1 � ��� 150 ���)
		voltage = (uint32_t) voltage_ADC * reference_U / reference_UADC; // ���������� ����, � | *���� (���������� ����������, � / voltage_ADC) (220 � ��� 900 ���)
		//-----------------------------------------
		switch (main_menu_state) // ������� �� ������ �������� ����
		{
			case MAIN_MENU_STATE_MM1_IDLE: // ����� 1 �������� ����
				strcpy_P(str_prog16, MMenuItem1); // �������� ������ �� flash � ������ ���
				sprintf(str_disp,"%s%3u`C",str_prog16,temper_set/100); // ������ ������ � �����
				direction_str_disp(str_disp); // ����� ������ ���� "�����������"
				main_menu_state = MAIN_MENU_STATE_MM1_WAIT; // ���������� ������ ����: "�������� ������ 1 ������"
				break;
			case MAIN_MENU_STATE_MM2_IDLE: // ����� 2 �������� ����
				strcpy_P(str_prog16, MMenuItem2); // �������� ������ �� flash � ������ ���
				sprintf(str_disp,"%s%4u ��/���",str_prog16,rpm_set); // ������ ������ � �����
				direction_str_disp(str_disp); // ����� ������ ���� "RPM"
				main_menu_state = MAIN_MENU_STATE_MM2_WAIT; // ���������� ������ ����: "�������� ������ 2 ������"
				break;
			case MAIN_MENU_STATE_MM3_IDLE: // ����� 3 �������� ����
				strcpy_P(str_prog16, MMenuItem3); // �������� ������ �� flash � ������ ���
				sprintf(str_disp,"%s%3u���*�",str_prog16,power_cons); // ������ ������ � �����
				direction_str_disp(str_disp); // ����� ������ ���� "������"
				main_menu_state = MAIN_MENU_STATE_MM3_WAIT; // ���������� ������ ����: "�������� ������ 3 ������"
				break;
			case MAIN_MENU_STATE_MM4_IDLE: // ����� 4 �������� ����
				strcpy_P(str_prog16, MMenuItem4); // �������� ������ �� flash � ������ ���
				direction_str_disp(str_prog16); // ����� ������ ���� "���� � �����"
				main_menu_state = MAIN_MENU_STATE_MM4_WAIT; // ���������� ������ ����: "�������� ������ 4 ������"
				break;
			case MAIN_MENU_STATE_MM5_IDLE: // ����� 5 �������� ����
				strcpy_P(str_prog16, MMenuItem5); // �������� ������ �� flash � ������ ���
				sprintf(str_disp,"%s%3u�",str_prog16,voltage); // ������ ������ � �����
				direction_str_disp(str_disp); // ����� ������ ���� "����������"
				main_menu_state = MAIN_MENU_STATE_MM5_WAIT; // ���������� ������ ����: "�������� ������ 5 ������"
				break;
			case MAIN_MENU_STATE_MM6_IDLE: // ����� 6 �������� ����
				strcpy_P(str_prog16, MMenuItem6); // �������� ������ �� flash � ������ ���
				sprintf(str_disp,"%s%01u.%02u�",str_prog16,current_total/100,current_total%100); // ������ ������ � �����
				direction_str_disp(str_disp); // ����� ������ ���� "���"
				main_menu_state = MAIN_MENU_STATE_MM6_WAIT; // ���������� ������ ����: "�������� ������ 6 ������"
				break;
			case MAIN_MENU_STATE_MM7_IDLE: // ����� 7 �������� ����
				strcpy_P(str_prog16, MMenuItem7); // �������� ������ �� flash � ������ ���
				sprintf(str_disp,"%s%3u.%01u%%",str_prog16,freememory/10,freememory%10); // ������ ������ � �����
				direction_str_disp(str_disp); // ����� ������ ���� "������:"
				main_menu_state = MAIN_MENU_STATE_MM7_WAIT; // ���������� ������ ����: "�������� ������ 7 ������"
				break;
			case MAIN_MENU_STATE_MM8_IDLE: // ����� 8 �������� ����
				strcpy_P(str_prog16, MMenuItem8); // �������� ������ �� flash � ������ ���
				direction_str_disp(str_prog16); // ����� ������ ���� "���������"
				main_menu_state = MAIN_MENU_STATE_MM8_WAIT; // ���������� ������ ����: "�������� ������ 8 ������"
				break;
			case MAIN_MENU_STATE_MM1_WAIT: // �������� ������ 1 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						main_menu_state = MAIN_MENU_STATE_MM2_IDLE; // ���������� ������ ����: "����� 2 �������� ����"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // ���������� ������ ����: "����� 8 �������� ����"
						break;
					case Butt_Select:
						main_menu_state = MAIN_MENU_STATE_MM1; // ���������� ������ ����: "����� ������ 1"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						menu_state = MENU_STATE_IDLE; // ���������� ������: "����� ���������"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM2_WAIT: // �������� ������ 2 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						main_menu_state = MAIN_MENU_STATE_MM3_IDLE; // ���������� ������ ����: "����� 3 �������� ����"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // ���������� ������ ����: "����� 1 �������� ����"
						break;
					case Butt_Select:
						main_menu_state = MAIN_MENU_STATE_MM2; // ���������� ������ ����: "����� ������ 2"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						menu_state = MENU_STATE_IDLE; // ���������� ������: "����� ���������"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM3_WAIT: // �������� ������ 3 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						main_menu_state = MAIN_MENU_STATE_MM2_IDLE; // ���������� ������ ����: "����� 2 �������� ����"
						break;
					case Butt_Select:
						main_menu_state = MAIN_MENU_STATE_MM3; // ���������� ������ ����: "����� ������ 3"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						menu_state = MENU_STATE_IDLE; // ���������� ������: "����� ���������"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM4_WAIT: // �������� ������ 4 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						main_menu_state = MAIN_MENU_STATE_MM5_IDLE; // ���������� ������ ����: "����� 5 �������� ����"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						main_menu_state = MAIN_MENU_STATE_MM3_IDLE; // ���������� ������ ����: "����� 3 �������� ����"
						break;
					case Butt_Select:
						direction = DIRECTION_RIGHT; // ��������� �������
						submenu_clock_state = SUBMENU_CLOCK_STATE_IDLE; // ���������� ������ ����: "������� "���� � �����""
						SubmenuClockProcess(); // ����� ������� ������� "����"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						menu_state = MENU_STATE_IDLE; // ���������� ������: "����� ���������"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM5_WAIT: // �������� ������ 5 ������
				strcpy_P(str_prog16, MMenuItem5); // �������� ������ �� flash � ������ ���
				sprintf(str_disp,"%s%3u�",str_prog16,voltage); // ������ ������ � �����
				hcms_puts(str_disp); // ����� ������ ���� "����������"
				//---------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						main_menu_state = MAIN_MENU_STATE_MM6_IDLE; // ���������� ������ ����: "����� 6 �������� ����"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						menu_state = MENU_STATE_IDLE; // ���������� ������: "����� ���������"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM6_WAIT: // �������� ������ 6 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						main_menu_state = MAIN_MENU_STATE_MM7_IDLE; // ���������� ������ ����: "����� 7 �������� ����"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						main_menu_state = MAIN_MENU_STATE_MM5_IDLE; // ���������� ������ ����: "����� 5 �������� ����"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						menu_state = MENU_STATE_IDLE; // ���������� ������: "����� ���������"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM7_WAIT: // �������� ������ 7 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // ���������� ������ ����: "����� 8 �������� ����"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						main_menu_state = MAIN_MENU_STATE_MM6_IDLE; // ���������� ������ ����: "����� 6 �������� ����"
						break;
					case Butt_Select:
						direction = DIRECTION_RIGHT; // ��������� �������
						submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "������:""
						SubmenuMemoryProcess(); // ����� ������� ������� "������:"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						menu_state = MENU_STATE_IDLE; // ���������� ������: "����� ���������"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM8_WAIT: // �������� ������ 8 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // ���������� ������ ����: "����� 1 �������� ����"
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						main_menu_state = MAIN_MENU_STATE_MM7_IDLE; // ���������� ������ ����: "����� 7 �������� ����"
						break;
					case Butt_Select:
						direction = DIRECTION_RIGHT; // ��������� �������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "���������""
						SubmenuSettingsProcess(); // ����� ������� ������� "���������"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						menu_state = MENU_STATE_IDLE; // ���������� ������: "����� ���������"
						return;
				}
				break;
			case MAIN_MENU_STATE_MM1: // ����� ������ 1
				//-----------------������ ���������--------------------
				strcpy_P(str_prog16, MMenuItem1); // �������� ������ �� flash � ������ ���
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
					sprintf(str_disp,"%s%3u`C",str_prog16,temper_set_tmp/100); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"%s   `C",str_prog16); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if(temper_set_tmp > TEMPER_SETMIN) // ����������� �� ����������� ����������� �������
							temper_set_tmp -= 100; // ��������� ����������� �������
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if(temper_set_tmp < TEMPER_SETMAX) // ����������� �� ������������ ����������� �������
							temper_set_tmp += 100; // ����������� ����������� �������
						break;
					case Butt_Select:
						temper_set = temper_set_tmp; // ��������� ���������� �������� ����������� �������
						EEPROM_write_word(ADDREEP_TEMPER_SET, temper_set_tmp); // �������� ����� (2 �����) � EEPROM �� ������
						main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // ���������� ������ ����: "����� 1 �������� ����"
						break;
					case Butt_Esc:
						temper_set_tmp = temper_set; // ���������� �������� ��������� ����������� ������� �� ��������� ����������
						main_menu_state = MAIN_MENU_STATE_MM1_IDLE; // ���������� ������ ����: "����� 1 �������� ����"
						break;
				}
				break;				
			case MAIN_MENU_STATE_MM2: // ����� ������ 2
				strcpy_P(str_prog16, MMenuItem2); // �������� ������ �� flash � ������ ���
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
					sprintf(str_disp,"%s%4u ��/���",str_prog16,rpm_set_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"%s     ��/���",str_prog16); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if(rpm_set_tmp > RPM_SETMIN) // ����������� �� ����������� RPM
							rpm_set_tmp -=100; // ��������� ������� ��������
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if(rpm_set_tmp < RPM_SETMAX) // ����������� �� ������������ RPM
							rpm_set_tmp += 100; // ����������� ������� ��������
						break;
					case Butt_Select:
						rpm_set = rpm_set_tmp; // ��������� ���������� �������� ����������� �������
						EEPROM_write_word(ADDREEP_RPM_SET, rpm_set_tmp); // �������� ����� (2 �����) � EEPROM �� ������
						main_menu_state = MAIN_MENU_STATE_MM2_IDLE; // ���������� ������ ����: "����� 2 �������� ����"
						break;
					case Butt_Esc:
						rpm_set_tmp = rpm_set; // ���������� �������� ��������� ����������� ������� �� ��������� ����������
						main_menu_state = MAIN_MENU_STATE_MM2_IDLE; // ���������� ������ ����: "����� 2 �������� ����"
						break;
				}
				break;
			case MAIN_MENU_STATE_MM3: // ����� ������ 3
				strcpy_P(str_prog16, MMenuItem3); // �������� ������ �� flash � ������ ���
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
					sprintf(str_disp,"%s%3u���*�",str_prog16,power_cons); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"%s   ���*�",str_prog16); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Select:
						power_cons = 0; // ���������� ������� ��������������
						main_menu_state = MAIN_MENU_STATE_MM3_IDLE; // ���������� ������ ����: "����� 3 �������� ����"
						break;
					case Butt_Esc:
						main_menu_state = MAIN_MENU_STATE_MM3_IDLE; // ���������� ������ ����: "����� 3 �������� ����"
						break;
				}
				break;
		}
	}
}

//-----------------------------------------------------
void SubmenuClockProcess(void) // ������� ������� "����"
{ 
	// ��������� ������� �������� �� ��������� ����������
	int8_t mday_tmp = mday,
		   month_tmp = month,
		   year_tmp = year,
		   wday_tmp = wday,
		   hour_tmp = hour,
		   minute_tmp = minute;
	while (TRUE)
	{
		switch (submenu_clock_state) // ������� �� ������ �������
		{
			case SUBMENU_CLOCK_STATE_IDLE: // ����� ������� "���� � �����"
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // �������� ������ �� flash � ������ ���
				sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
				direction_str_disp(str_disp); // ����� ������ ������� "���� � �����"
				submenu_clock_state = SUBMENU_CLOCK_STATE_HOUR; // ���������� ������ ����: "�������������� ����"
				break;
			case SUBMENU_CLOCK_STATE_HOUR: // �������������� ����
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // �������� ������ �� flash � ������ ���
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TICK, �� ������� ��������
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"  :%02u %s %2u%s%02u",minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						hour_tmp--; // ���������
						if(hour_tmp<0) hour_tmp = 23;
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						hour_tmp++; // �����������
						if(hour_tmp>23) hour_tmp = 0;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MINUTE; // ���������� ������ ����: "�������������� ������"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_YEAR; // ���������� ������ ����: "�������������� ����"
						break;
					case Butt_Select:
						hour = hour_tmp;     // ��������� ���������� ��������
						minute = minute_tmp; // ��������� ���������� ��������
						wday = wday_tmp;     // ��������� ���������� ��������
						mday = mday_tmp;     // ��������� ���������� ��������
						month = month_tmp;   // ��������� ���������� ��������
						year = year_tmp;     // ��������� ���������� ��������
						second = 0; // �������� �������
						DS1307_Set_Time(); // ���������� ����� (������ �����)
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_MINUTE: // �������������� ������
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // �������� ������ �� flash � ������ ���
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TICK, �� ������� ��������
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"%2u:   %s %2u%s%02u",hour_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						minute_tmp--; // ���������
						if(minute_tmp<0) minute_tmp = 59;
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						minute_tmp++; // �����������
						if(minute_tmp>59) minute_tmp = 0;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_WDAY; // ���������� ������ ����: "�������������� ��� ������"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_HOUR; // ���������� ������ ����: "�������������� ����"
						break;
					case Butt_Select:
						hour = hour_tmp;     // ��������� ���������� ��������
						minute = minute_tmp; // ��������� ���������� ��������
						wday = wday_tmp;     // ��������� ���������� ��������
						mday = mday_tmp;     // ��������� ���������� ��������
						month = month_tmp;   // ��������� ���������� ��������
						year = year_tmp;     // ��������� ���������� ��������
						second = 0; // �������� �������
						DS1307_Set_Time(); // ���������� ����� (������ �����)
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_WDAY: // �������������� ��� ������
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // �������� ������ �� flash � ������ ���
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TICK, �� ������� ��������
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"%2u:%02u    %2u%s%02u",hour_tmp,minute_tmp,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						wday_tmp--; // ���������
						if(wday_tmp<1) wday_tmp = 7;
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						wday_tmp++; // �����������
						if(wday_tmp>7) wday_tmp = 1;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MDAY; // ���������� ������ ����: "�������������� ��� ������"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MINUTE; // ���������� ������ ����: "�������������� ������"
						break;
					case Butt_Select:
						hour = hour_tmp;     // ��������� ���������� ��������
						minute = minute_tmp; // ��������� ���������� ��������
						wday = wday_tmp;     // ��������� ���������� ��������
						mday = mday_tmp;     // ��������� ���������� ��������
						month = month_tmp;   // ��������� ���������� ��������
						year = year_tmp;     // ��������� ���������� ��������
						second = 0; // �������� �������
						DS1307_Set_Time(); // ���������� ����� (������ �����)
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_MDAY: // �������������� ��� ������
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // �������� ������ �� flash � ������ ���
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TICK, �� ������� ��������
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"%2u:%02u %s   %s%02u",hour_tmp,minute_tmp,str_prog3,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						mday_tmp--; // ���������
						if(mday_tmp<1) mday_tmp = 31;
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						mday_tmp++; // �����������
						if(mday_tmp>31) mday_tmp = 1;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MONTH; // ���������� ������ ����: "�������������� ������"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_WDAY; // ���������� ������ ����: "�������������� ��� ������"
						break;
					case Butt_Select:
						hour = hour_tmp;     // ��������� ���������� ��������
						minute = minute_tmp; // ��������� ���������� ��������
						wday = wday_tmp;     // ��������� ���������� ��������
						mday = mday_tmp;     // ��������� ���������� ��������
						month = month_tmp;   // ��������� ���������� ��������
						year = year_tmp;     // ��������� ���������� ��������
						second = 0; // �������� �������
						DS1307_Set_Time(); // ���������� ����� (������ �����)
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_MONTH: // �������������� ������
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // �������� ������ �� flash � ������ ���
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TICK, �� ������� ��������
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"%2u:%02u %s %2u   %02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						month_tmp--; // ���������
						if(month_tmp<1) month_tmp = 12;
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						month_tmp++; // �����������
						if(month_tmp>12) month_tmp = 1;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_YEAR; // ���������� ������ ����: "�������������� ����"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MDAY; // ���������� ������ ����: "�������������� ��� ������"
						break;
					case Butt_Select:
						hour = hour_tmp;     // ��������� ���������� ��������
						minute = minute_tmp; // ��������� ���������� ��������
						wday = wday_tmp;     // ��������� ���������� ��������
						mday = mday_tmp;     // ��������� ���������� ��������
						month = month_tmp;   // ��������� ���������� ��������
						year = year_tmp;     // ��������� ���������� ��������
						second = 0; // �������� �������
						DS1307_Set_Time(); // ���������� ����� (������ �����)
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
				}
				break;
			case SUBMENU_CLOCK_STATE_YEAR: // �������������� ����
				strcpy_P(str_prog3,(PGM_P)pgm_read_word(&(string_tableDay[wday-1]))); // �������� ������ �� flash � ������ ���
				strcpy_P(str_prog16,(PGM_P)pgm_read_word(&(string_tableMonth[month_tmp-1]))); // �������� ������ �� flash � ������ ���
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TICK, �� ������� ��������
					sprintf(str_disp,"%2u:%02u %s %2u%s%02u",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16,year_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"%2u:%02u %s %2u%s",hour_tmp,minute_tmp,str_prog3,mday_tmp,str_prog16); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						year_tmp--; // ���������
						if(year_tmp<0) year_tmp = 99;
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						year_tmp++; // �����������
						if(year_tmp>99) year_tmp = 0;
						break;
					case Butt_Right:
						submenu_clock_state = SUBMENU_CLOCK_STATE_HOUR; // ���������� ������ ����: "�������������� ����"
						break;
					case Butt_Left:
						submenu_clock_state = SUBMENU_CLOCK_STATE_MONTH; // ���������� ������ ����: "�������������� ������"
						break;
					case Butt_Select:
						hour = hour_tmp;     // ��������� ���������� ��������
						minute = minute_tmp; // ��������� ���������� ��������
						wday = wday_tmp;     // ��������� ���������� ��������
						mday = mday_tmp;     // ��������� ���������� ��������
						month = month_tmp;   // ��������� ���������� ��������
						year = year_tmp;     // ��������� ���������� ��������
						second = 0; // �������� �������
						DS1307_Set_Time(); // ���������� ����� (������ �����)
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM4_IDLE; // ���������� ������ ����: "����� 4 �������� ����"
						return;
				}
				break;
		}
	}
}

//-----------------------------------------------------
void SubmenuMemoryProcess(void) // ������� ������� "������:"
{ 
	uint8_t yes_flag = FALSE; // ���� �����������, ���� ���������� ����� "��" � ����������, ���� ���������� ����� "���".
	uint8_t cpy_erase = COPY; // ���� �������� ����� "�����������" cpy_erase = COPY; ���� �������� ����� "��������" cpy_erase = EERASE.
	while (TRUE)
	{
		switch (submenu_memory_state) // ������� �� ������ �������
		{
			case SUBMENU_MEMORY_STATE_M1_IDLE: // ����� 1 ������� "������:"
				strcpy_P(str_prog16, MemoryItem1); // �������� ������ �� flash � ������ ���
				direction_str_disp(str_prog16); // ����� ������ ���� "���������� �� SD"
				submenu_memory_state = SUBMENU_MEMORY_STATE_M1_WAIT; // ���������� ������ ����: "�������� ������ 1 ������"
				break;
			case SUBMENU_MEMORY_STATE_M2_IDLE: // ����� 2 ������� "������:"
				strcpy_P(str_prog16, MemoryItem2); // �������� ������ �� flash � ������ ���
				direction_str_disp(str_prog16); // ����� ������ ���� "�������� ������"
				submenu_memory_state = SUBMENU_MEMORY_STATE_M2_WAIT; // ���������� ������ ����: "�������� ������ 2 ������"
				break;
			case SUBMENU_MEMORY_STATE_M1_WAIT: // �������� ������ 1 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "������:""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "������:""
						break;
					case Butt_Select:
						cpy_erase = COPY; // �������� ����� "�����������"
						submenu_memory_state = SUBMENU_MEMORY_STATE_YESNO_WAIT; // ���������� ������ ����: "�������� ������ "��" ��� "���""
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM7_IDLE; // ���������� ������ ����: "����� 7 �������� ����"
						return;
				}
				break;
			case SUBMENU_MEMORY_STATE_M2_WAIT: // �������� ������ 2 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "������:""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "������:""
						break;
					case Butt_Select:
						cpy_erase = ERASE; // �������� ����� "��������"
						submenu_memory_state = SUBMENU_MEMORY_STATE_YESNO_WAIT; // ���������� ������ ����: "�������� ������ "��" ��� "���""
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM7_IDLE; // ���������� ������ ����: "����� 7 �������� ����"
						return;
				}
				break;
			case SUBMENU_MEMORY_STATE_YESNO_WAIT: // �������� ������ "��" ��� "���"
				//-----------------������ ���������--------------------
				if (yes_flag){ // ���� ������ ����� "��"
					// ������ ������� "��":
					strcpy_P(str_prog16, MemoryItem3); // �������� ������ �� flash � ������ ���
					if(tim_cnt < BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
						hcms_puts(str_prog16); // ����� ������ �� �������
					}
					else {
						hcms_puts("          ���"); // ����� ������ �� �������
					}
				}
				else {  // ���� ������ ����� "���"
					// ������ ������� "���":
					strcpy_P(str_prog16, MemoryItem3); // �������� ������ �� flash � ������ ���
					if(tim_cnt < BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
						hcms_puts(str_prog16); // ����� ������ �� �������
					}
					else {
						hcms_puts("   ��"); // ����� ������ �� �������
					}
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Right:
						yes_flag = !yes_flag; // ����������� ����������
						break;
					case Butt_Left:
						yes_flag = !yes_flag; // ����������� ����������
						break;
					case Butt_Select:
						if (yes_flag) // ���� ������ ����� "��"
						{
							if (cpy_erase == COPY) // ���� ������ ����� "�����������"
								submenu_memory_state = SUBMENU_MEMORY_STATE_COPYPROGRESS; // ���������� ������ ����: "�����������"
							else if (cpy_erase == ERASE) // ���� ������ ����� "��������"
								submenu_memory_state = SUBMENU_MEMORY_STATE_ERASEPROGRESS; // ���������� ������ ����: "��������"
						}
						else // ���� ������ ����� "���"
						{
							if (cpy_erase == COPY) // ���� ������ ����� "�����������"
								submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "������:""
							else if (cpy_erase == ERASE) // ���� ������ ����� "��������"
								submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "������:""
						}
						yes_flag = FALSE; // �������� ����
						break;
					case Butt_Esc:
						if (cpy_erase == COPY) // ���� ������ ����� "�����������"
							submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "������:""
						else if (cpy_erase == ERASE) // ���� ������ ����� "��������"
							submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "������:""
						yes_flag = FALSE; // �������� ����
						break;
				}
				break;
			case SUBMENU_MEMORY_STATE_COPYPROGRESS: // �����������
				// ���������� ���� �� SD-����� �� ������ 0
				// ������ ���� � SD-����� �� ������ 0
				// ���� ���� �������� ����� (��������, ��� SD-����� ������������ � �����)
					// ������� ���� ���� � SD-�����
					// ���������� � �����������
					// ������� ������ ����������� "��������: 100%"
					// ���� ����������� ��������� �������
						// ������� ��������� "  �����������   "
						// ��������� � ������ ���� "���� ������� ����� ������"
					// �����, ���� ����������� ��������� �� �������
						// ������� ��������� "   Copy error   "
						// ��������� � ������ ���� "���� ������� ����� ������"
				// �����, ���� ��� SD-�����
					// ������� ��������� "��� SD �����"
					// ��������� � ������ ���� "���� ������� ����� ������"
					
					strcpy_P(str_prog16, MemoryItem6); // �������� ������ �� flash � ������ ���
					hcms_puts(str_prog16); // ������� ��������� "��� SD �����"
					submenu_memory_state = SUBMENU_MEMORY_STATE_WAIT; // ���������� ������ ����: "���� ������� ����� ������"
				break;
			case SUBMENU_MEMORY_STATE_WAIT: // ���� ������� ����� ������
				if(KEYB_GetKey()) // ���� ��������� ������� ����� ������
				{
					if (cpy_erase == COPY) // ���� ������ ����� "�����������"
						submenu_memory_state = SUBMENU_MEMORY_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "������:""
					else if (cpy_erase == ERASE) // ���� ������ ����� "��������"
						submenu_memory_state = SUBMENU_MEMORY_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "������:""
				}
				break;
			case SUBMENU_MEMORY_STATE_ERASEPROGRESS: // ��������
				// ������� ������ �������� "��������: 100%"
				// ���������� ��� 0x55
				// ���� ������ ������ �������
					// ������ ������
					// ���� ������ ��������� �������
						// ������� ������
						// ���� �������� ��������� �������
							// ���� ���������� ������ 0 b
								// ������� ��������� "���������� 999 b"
								// ��������� � ������ ���� "���� ������� ����� ������"
							// �����
								// ������� ��������� "������ �������"
								// ��������� � ������ ���� "���� ������� ����� ������"
						// �����
							// ������� ��������� "Bad memory"
							// ��������� � ������ ���� "���� ������� ����� ������"
					// �����
						// ������� ��������� "Bad memory"
						// ��������� � ������ ���� "���� ������� ����� ������"
				// �����
					// ������� ��������� "Bad memory"
					// ��������� � ������ ���� "���� ������� ����� ������"
					
					strcpy_P(str_prog16, MemoryItem8); // �������� ������ �� flash � ������ ���
					hcms_puts(str_prog16); // ������� ��������� "   Bad memory   "
					submenu_memory_state = SUBMENU_MEMORY_STATE_WAIT; // ���������� ������ ����: "���� ������� ����� ������"
				break;
		}
	}
}

//-----------------------------------------------------
void SubmenuSettingsProcess(void) // ������� ������� "���������"
{
	uint8_t brightness_seg_tmp = brightness_seg; // ������� 7-����������� ����������, % (��� ��������� 2 %)
	uint8_t brightness_led_tmp = brightness_led; // ������� LED-����������, % (��� ��������� 2 %)
	uint8_t brightness_led_p = brightness_led_tmp; // �������� ��� ������������� ��������� ������� LED-����������
	uint8_t reference_I_tmp = reference_I; // ���������� ��� ��������������� �����������, *100�
	uint8_t reference_U_tmp = reference_U; // ���������� ���������� ��������������� �����������, �
	uint16_t reference_IADC_tmp = reference_IADC; // ���������� �������� I ��� ��� reference_I, ���
	uint16_t reference_UADC_tmp = reference_UADC; // ���������� �������� U ��� ��� reference_U, ���
	// ���������� ��� ������ ���� "���������� I":
	extern uint16_t temper_realgrad; // ���������� �����������, [2271;65535] *100'C
 	while (TRUE)
	{
		if (brightness_led_p != brightness_led_tmp){ // ���� ������� LED-���������� ��������
			if (brightness_led_tmp < 16){
				hcms_peak_current(PEAK_04_MA); // ��������� ������� ��� �������� 4.0 mA (31% �������)
				hcms_bright(brightness_led_tmp%16); // ��������� �������
			}
			else if (brightness_led_tmp >= 16 && brightness_led_tmp < 32){
				hcms_peak_current(PEAK_06_MA); // ��������� ������� ��� �������� 6.4 mA (50% �������)
				hcms_bright(brightness_led_tmp%16); // ��������� �������
			}
			else if (brightness_led_tmp >= 32 && brightness_led_tmp < 48){
				hcms_peak_current(PEAK_09_MA); // ��������� ������� ��� �������� 9.3 mA (73% �������)
				hcms_bright(brightness_led_tmp%16); // ��������� �������
			}
			else if (brightness_led_tmp >= 48 && brightness_led_tmp < 64){
				hcms_peak_current(PEAK_13_MA); // ��������� ������� ��� �������� 12.8 mA (100% �������)
				hcms_bright(brightness_led_tmp%16); // ��������� �������
			}
			brightness_led_p = brightness_led_tmp; // ��������
		}
 		switch (submenu_settings_state) // ������� �� ������ �������
 		{
 			case SUBMENU_SETTINGS_STATE_M1_IDLE: // ����� 1 ������� "���������"
				strcpy_P(str_prog16, SettingsItem1); // �������� ������ �� flash � ������ ���
				direction_str_disp(str_prog16); // ����� ������ ���� "������� 7-����."
				submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_WAIT; // ���������� ������ ����: "�������� ������ 1 ������"
				break;
 			case SUBMENU_SETTINGS_STATE_M2_IDLE: // ����� 2 ������� "���������"
				strcpy_P(str_prog16, SettingsItem2); // �������� ������ �� flash � ������ ���
				direction_str_disp(str_prog16); // ����� ������ ���� "������� LED ���."
				submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_WAIT; // ���������� ������ ����: "�������� ������ 2 ������"
				break;
 			case SUBMENU_SETTINGS_STATE_M3_IDLE: // ����� 3 ������� "���������"
				strcpy_P(str_prog16, SettingsItem3); // �������� ������ �� flash � ������ ���
				direction_str_disp(str_prog16); // ����� ������ ���� "���������� U"
				submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_WAIT; // ���������� ������ ����: "�������� ������ 3 ������"
				break;
			case SUBMENU_SETTINGS_STATE_M4_IDLE: // ����� 4 ������� "���������"
				strcpy_P(str_prog16, SettingsItem4); // �������� ������ �� flash � ������ ���
				direction_str_disp(str_prog16); // ����� ������ ���� "���������� I"
				submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_WAIT; // ���������� ������ ����: "�������� ������ 4 ������"
				break;
			case SUBMENU_SETTINGS_STATE_M1_WAIT: // �������� ������ 1 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "���������""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_IDLE; // ���������� ������ ����: "����� 4 ������� "���������""
						break;
					case Butt_Select:
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1; // ���������� ������ ����: "����� ������ 1"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // ���������� ������ ����: "����� 8 �������� ����"
						return;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M2_WAIT: // �������� ������ 2 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_IDLE; // ���������� ������ ����: "����� 3 ������� "���������""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "���������""
						break;
					case Butt_Select:
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2; // ���������� ������ ����: "����� ������ 2"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // ���������� ������ ����: "����� 8 �������� ����"
						return;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M3_WAIT: // �������� ������ 3 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_IDLE; // ���������� ������ ����: "����� 4 ������� "���������""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "���������""
						break;
					case Butt_Select:
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3; // ���������� ������ ����: "����� ������ 3"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // ���������� ������ ����: "����� 8 �������� ����"
						return;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M4_WAIT: // �������� ������ 4 ������
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						direction = DIRECTION_DOWN; // ��������� ����
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "���������""
						break;
					case Butt_Up:
						direction = DIRECTION_UP; // ��������� �����
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_IDLE; // ���������� ������ ����: "����� 3 ������� "���������""
						break;
					case Butt_Select:
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4; // ���������� ������ ����: "����� ������ 4"
						break;
					case Butt_Esc:
						direction = DIRECTION_LEFT; // ��������� ������
						main_menu_state = MAIN_MENU_STATE_MM8_IDLE; // ���������� ������ ����: "����� 8 �������� ����"
						return;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M1: // ����� ������ 1
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
					sprintf(str_disp,"%3u%%",brightness_seg_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					hcms_puts("   %"); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if (brightness_seg_tmp > 0)
							brightness_seg_tmp -= 2; // ��������� �������
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if (brightness_seg_tmp < 100)
							brightness_seg_tmp += 2; // ����������� �������
						break;
					case Butt_Select:
						brightness_seg = brightness_seg_tmp; // ��������� ���������� �������� �������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "���������""
						break;
					case Butt_Esc:
						brightness_seg_tmp = brightness_seg; // ���������� �������� ��������� ������� �� ��������� ����������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M1_IDLE; // ���������� ������ ����: "����� 1 ������� "���������""
						break;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M2: // ����� ������ 2
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
					sprintf(str_disp,"%3u%%",brightness_led_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					hcms_puts("   %"); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if (brightness_led_tmp > 0)
							brightness_led_tmp --; // ��������� �������
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if (brightness_led_tmp < 63)
							brightness_led_tmp ++; // ����������� �������
						break;
					case Butt_Select:
						brightness_led = brightness_led_tmp; // ��������� ���������� �������� �������
						EEPROM_write_byte(ADDREEP_BR_LED, brightness_led_tmp); // �������� ���� � EEPROM �� ������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "���������""
						break;
					case Butt_Esc:
						brightness_led_tmp = brightness_led; // ���������� �������� ��������� ������� �� ��������� ����������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M2_IDLE; // ���������� ������ ����: "����� 2 ������� "���������""
						break;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M3: // ����� ������ 3
				reference_UADC_tmp = adc_data(ADC_CH_2); // ������ �������� ��� (����������)
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
					sprintf(str_disp,"U=%3u�  ADC=%u",reference_U_tmp,reference_UADC_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"U=   �  ADC=%u",reference_UADC_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if (reference_U_tmp > 0)
							reference_U_tmp--; // ��������� �������
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if (reference_U_tmp < EEP_CLEN_BYTE - 1) // �.�. 255 - ������ ������ � EEPROM
							reference_U_tmp++; // ����������� �������� ����������� ����������
						break;
					case Butt_Select:
						reference_U = reference_U_tmp; // ��������� ���������� �������� ����������� ����������
						reference_UADC = reference_UADC_tmp; // ��������� ���������� �������� ���
						EEPROM_write_byte(ADDREEP_REF_U, reference_U_tmp); // �������� ���� � EEPROM �� ������
						EEPROM_write_word(ADDREEP_REF_UADC, reference_UADC_tmp); // �������� ����� (2 �����) � EEPROM �� ������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_IDLE; // ���������� ������ ����: "����� 3 ������� "���������""
						break;
					case Butt_Esc:
						reference_U_tmp = reference_U; // ���������� �������� ��������� ����������� ���������� �� ��������� ����������
						reference_UADC_tmp = reference_UADC; // ���������� �������� ��������� ����������� �������� ��� �� ��������� ����������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M3_IDLE; // ���������� ������ ����: "����� 3 ������� "���������""
						break;
				}
				break;
			case SUBMENU_SETTINGS_STATE_M4: // ����� ������ 4
				temper_realgrad = pgm_read_word(&table_termores[adc_data(ADC_CH_1)]); // ��������� ����������� 'C �� ����������� �������� ���
				if (temper_realgrad < 12000){ // ���� ���� �� ��������� �� 120 'C
					TEN_ON; // �������� ���
					LED_TEN_ON; // �������� ��������� ���
				}
				else {
					TEN_OFF; // ��������� ���
					LED_TEN_OFF; // ��������� ��������� ���
				}
				reference_IADC_tmp = adc_data(ADC_CH_0); // ������ �������� ��� (���)
				//-----------------������ ���������--------------------
				if(tim_cnt<BURNING_TICK) { // ���� ������� ������ BURNING_TIME, �� ������� ��������
					sprintf(str_disp,"I=%01u.%02u� ADC=%u",reference_I_tmp/100,reference_I_tmp%100,reference_IADC_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				else {
					sprintf(str_disp,"I=    � ADC=%u",reference_IADC_tmp); // ������ ������ � �����
					hcms_puts(str_disp); // ����� ������ �� �������
				}
				//-----------------------------------------------------
				switch (KEYB_GetKey())
				{
					case Butt_Down:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if (reference_I_tmp > 0)
							reference_I_tmp--; // ��������� �������� ����������� ����
						break;
					case Butt_Up:
						but_press = TRUE; // ��������� ������������ "�������" �� ������
						if (reference_I_tmp < EEP_CLEN_BYTE - 1) // �.�. 255 - ������ ������ � EEPROM
							reference_I_tmp++; // ����������� �������� ����������� ����
						break;
					case Butt_Select:
						TEN_OFF; // ��������� ���
						LED_TEN_OFF; // ��������� ��������� ���
						reference_I = reference_I_tmp; // ��������� ���������� �������� ����������� ����
						reference_IADC = reference_IADC_tmp; // ��������� ���������� �������� ���
						EEPROM_write_byte(ADDREEP_REF_I, reference_I_tmp); // �������� ���� � EEPROM �� ������
						EEPROM_write_word(ADDREEP_REF_IADC, reference_IADC_tmp); // �������� ����� (2 �����) � EEPROM �� ������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_IDLE; // ���������� ������ ����: "����� 4 ������� "���������""
						break;
					case Butt_Esc:
						TEN_OFF; // ��������� ���
						LED_TEN_OFF; // ��������� ��������� ���
						reference_I_tmp = reference_I; // ���������� �������� ��������� ����������� ���������� �� ��������� ����������
						reference_IADC_tmp = reference_IADC; // ���������� �������� ��������� ����������� �������� ��� �� ��������� ����������
						submenu_settings_state = SUBMENU_SETTINGS_STATE_M4_IDLE; // ���������� ������ ����: "����� 4 ������� "���������""
						break;
				}
				break;
 		}
 	}
}