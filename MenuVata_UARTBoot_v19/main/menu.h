#ifndef MENU_H_
#define MENU_H_
//-----------------------------------------------------------
#include "main.h"
//-----------------------------------------------------------
// ��������� ������� ������� �������� ����
#define BURNING_TIME 80 // ����� �������� ��� ������� (% �� 1 ���.)
#define BURNING_TICK (TICKSEC*BURNING_TIME/100) // ����� �������� ��� ������� (� �����)
//-----------------------------------------------------------
#define RPM_SETMAX 2800 // ������������ ������� ��������, ��/���
#define RPM_SETMIN 200 // ����������� ������� ��������, ��/���
#define TEMPER_SETMAX 25000 // ������������ ����������� �������, '�/100
#define TEMPER_SETMIN 2000 // ����������� ����������� �������, '�/100
//-----------------------------------------------------------
enum { 
	COPY,  // ���������� ����������� ������
	ERASE // ���������� �������� ������
};
// ����������� �������� �� ����
enum { // ��������� � ������������� ��� �������� ����������� �������� �� ����
	DIRECTION_NO, // ��� ����������� ��������
	DIRECTION_UP,   // ����� �����
	DIRECTION_DOWN, // ������ ����
	DIRECTION_LEFT, // ����� ������
	DIRECTION_RIGHT // ������ �������
};
//-----------------------------------------------------------
// NAME_IDLE - ����� ������ ���� (���� ���)
// NAME_WAIT - ������� ������� ������ (����)
// NAME      - �������� ��������
typedef enum { // ��������� � ������������� ��� ������ ����
	MENU_STATE_IDLE, // ����� ���������
	MENU_STATE_WAIT, // ���� ������� �������� ���� (������� ������)
	MENU_STATE_MAIN  // ������ �������� ����
}MENU_StateTypeDef;
//-----------------------------------------------------------
typedef enum { // ��������� � ������������� ��� �������� ����
	MAIN_MENU_STATE_MM1_IDLE, // ����� 1 �������� ����
	MAIN_MENU_STATE_MM2_IDLE, // ����� 2 �������� ����
	MAIN_MENU_STATE_MM3_IDLE, // ����� 3 �������� ����
	MAIN_MENU_STATE_MM4_IDLE, // ����� 4 �������� ����
	MAIN_MENU_STATE_MM5_IDLE, // ����� 5 �������� ����
	MAIN_MENU_STATE_MM6_IDLE, // ����� 6 �������� ����
	MAIN_MENU_STATE_MM7_IDLE, // ����� 7 �������� ����
	MAIN_MENU_STATE_MM8_IDLE, // ����� 8 �������� ����
	MAIN_MENU_STATE_MM1_WAIT, // �������� ������ 1 ������
	MAIN_MENU_STATE_MM2_WAIT, // �������� ������ 2 ������
	MAIN_MENU_STATE_MM3_WAIT, // �������� ������ 3 ������
	MAIN_MENU_STATE_MM4_WAIT, // �������� ������ 4 ������
	MAIN_MENU_STATE_MM5_WAIT, // �������� ������ 5 ������
	MAIN_MENU_STATE_MM6_WAIT, // �������� ������ 6 ������
	MAIN_MENU_STATE_MM7_WAIT, // �������� ������ 7 ������
	MAIN_MENU_STATE_MM8_WAIT, // �������� ������ 7 ������
	MAIN_MENU_STATE_MM1, // ����� ������ 1
	MAIN_MENU_STATE_MM2, // ����� ������ 2
	MAIN_MENU_STATE_MM3  // ����� ������ 3
}MAIN_MENU_StateTypeDef;
//-----------------------------------------------------------
typedef enum { // ��������� � ������������� ��� ������� "����"
	SUBMENU_CLOCK_STATE_IDLE, // ������� "���� � �����"
	SUBMENU_CLOCK_STATE_HOUR,   // �������������� ����
	SUBMENU_CLOCK_STATE_MINUTE, // �������������� ������
	SUBMENU_CLOCK_STATE_WDAY,   // �������������� ��� ������
	SUBMENU_CLOCK_STATE_MDAY,   // �������������� ��� ������
	SUBMENU_CLOCK_STATE_MONTH,  // �������������� ������
	SUBMENU_CLOCK_STATE_YEAR    // �������������� ����
}SUBMENU_CLOCK_StateTypeDef;
//-----------------------------------------------------------
typedef enum { // ��������� � ������������� ��� ������� "������:"
	SUBMENU_MEMORY_STATE_M1_IDLE, // ����� 1 ������� "������:"
	SUBMENU_MEMORY_STATE_M2_IDLE, // ����� 2 ������� "������:"
	SUBMENU_MEMORY_STATE_M1_WAIT, // �������� ������ 1 ������
	SUBMENU_MEMORY_STATE_M2_WAIT, // �������� ������ 2 ������
	SUBMENU_MEMORY_STATE_YESNO_WAIT, // �������� ������ "��" ��� "���"
	SUBMENU_MEMORY_STATE_COPYPROGRESS, // ��������: 100% (�����������)
	SUBMENU_MEMORY_STATE_ERASEPROGRESS, // ��������: 100% (��������)
	SUBMENU_MEMORY_STATE_WAIT // ���� ������� ����� ������
}SUBMENU_MEMORY_StateTypeDef;
//-----------------------------------------------------------
typedef enum { // ��������� � ������������� ��� ������� "���������"
	SUBMENU_SETTINGS_STATE_M1_IDLE, // ����� 1 ������� "���������"
	SUBMENU_SETTINGS_STATE_M2_IDLE, // ����� 2 ������� "���������"
	SUBMENU_SETTINGS_STATE_M3_IDLE, // ����� 3 ������� "���������"
	SUBMENU_SETTINGS_STATE_M4_IDLE, // ����� 4 ������� "���������"
	SUBMENU_SETTINGS_STATE_M1_WAIT, // �������� ������ 1 ������
	SUBMENU_SETTINGS_STATE_M2_WAIT, // �������� ������ 2 ������
	SUBMENU_SETTINGS_STATE_M3_WAIT, // �������� ������ 3 ������
	SUBMENU_SETTINGS_STATE_M4_WAIT, // �������� ������ 4 ������
	SUBMENU_SETTINGS_STATE_M1, // ����� ������ 1
	SUBMENU_SETTINGS_STATE_M2, // ����� ������ 2
	SUBMENU_SETTINGS_STATE_M3, // ����� ������ 3
	SUBMENU_SETTINGS_STATE_M4 // ����� ������ 4
}SUBMENU_SETTINGS_StateTypeDef;
//-----------------------------------------------------------
void MenuProcess(void); // ����� ������� ����
//-----------------------------------------------------------
#endif /* MENU_H_ */