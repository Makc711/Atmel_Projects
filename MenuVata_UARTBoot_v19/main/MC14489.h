#ifndef MC14489_H_
#define MC14489_H_

#include "main.h"

//========================================
// Определение портов и пинов:
#define PORT_CLOCK   D
#define PIN_CLOCK    5

#define PORT_ENABLE  E
#define PIN_ENABLE   7

#define PORT_DATAIN  B
#define PIN_DATAIN   2
//========================================
// Порядок следования индикаторов (справа налево):
#define BANKnum 54321 // Номера индикаторов, которые не используются записывать в начало
//========================================
//----------------------------------------
#define _CLOCK_0   PORT(PORT_CLOCK) &= ~_BV(PIN_CLOCK)
#define _CLOCK_1   PORT(PORT_CLOCK) |= _BV(PIN_CLOCK)
#define _ENABLE_0  PORT(PORT_ENABLE) &= ~_BV(PIN_ENABLE)
#define _ENABLE_1  PORT(PORT_ENABLE) |= _BV(PIN_ENABLE)
#define _DATAIN_0  PORT(PORT_DATAIN) &= ~_BV(PIN_DATAIN)
#define _DATAIN_1  PORT(PORT_DATAIN) |= _BV(PIN_DATAIN)
//----------------------------------------
//========================================
// БИБЛИОТЕКА СИМВОЛОВ MC14489:
// Hex Decode:
#define Sym_0	  0x00
#define Sym_1	  0x01
#define Sym_2	  0x02
#define Sym_3	  0x03
#define Sym_4	  0x04
#define Sym_5	  0x05 // Sym_S
#define Sym_6	  0x06
#define Sym_7	  0x07
#define Sym_8	  0x08 // Sym_B
#define Sym_9	  0x09 // Sym_g
#define Sym_A	  0x0A
#define Sym_b     0x0B
#define Sym_C	  0x0C
#define Sym_d	  0x0D
#define Sym_E	  0x0E
#define Sym_F	  0x0F
// Special Decode:
#define Sym__	  0x00 // Пробел
#define Sym_c	  0x01
#define Sym_H	  0x02
#define Sym_h	  0x03
#define Sym_J	  0x04
#define Sym_L	  0x05
#define Sym_n	  0x06
#define Sym_o	  0x07
#define Sym_P	  0x08
#define Sym_r	  0x09
#define Sym_U	  0x0A
#define Sym_u     0x0B
#define Sym_Y	  0x0C
#define Sym_tire  0x0D
#define Sym_ravno 0x0E
#define Sym_grad  0x0F
//========================================
// БИБЛИОТЕКА СИМВОЛОВ ASCII:
// Hex Decode:
#define ASCII_Sym_0	  0x30
#define ASCII_Sym_1	  0x31
#define ASCII_Sym_2	  0x32
#define ASCII_Sym_3	  0x33
#define ASCII_Sym_4	  0x34
#define ASCII_Sym_5	  0x35
#define ASCII_Sym_6	  0x36
#define ASCII_Sym_7	  0x37
#define ASCII_Sym_8	  0x38
#define ASCII_Sym_9	  0x39
#define ASCII_Sym_A	  0x41
#define ASCII_Sym_b   0x62
#define ASCII_Sym_C	  0x43
#define ASCII_Sym_d	  0x64
#define ASCII_Sym_E	  0x45
#define ASCII_Sym_F	  0x46
// Special Decode:
#define ASCII_Sym__	    0x20 // Пробел
#define ASCII_Sym_c	    0x63
#define ASCII_Sym_H	    0x48
#define ASCII_Sym_h	    0x68
#define ASCII_Sym_J	    0x4A
#define ASCII_Sym_L	    0x4C
#define ASCII_Sym_n	    0x6E
#define ASCII_Sym_o	    0x6F
#define ASCII_Sym_P	    0x50
#define ASCII_Sym_r	    0x72
#define ASCII_Sym_U	    0x55
#define ASCII_Sym_u     0x75
#define ASCII_Sym_Y	    0x59
#define ASCII_Sym_tire  0x2D // Символ -
#define ASCII_Sym_ravno 0x3D // Символ =
#define ASCII_Sym_grad  0x27 // Символ '
// Дополнительно:
#define ASCII_Sym_S	  0x53
#define ASCII_Sym_B	  0x42
#define ASCII_Sym_g	  0x67
#define ASCII_Sym_O	  0x4F
//========================================
// Позиция точкии, уровень яркости индикаторов:
// L - 50% яркости индикаторов:
#define L_Alloff  0x00 // Все точки не горят
#define L_BANK1   0x01 // Точка в индикаторе 1
#define L_BANK2   0x02 // Точка в индикаторе 2
#define L_BANK3   0x03 // Точка в индикаторе 3
#define L_BANK4   0x04 // Точка в индикаторе 4
#define L_BANK5   0x05 // Точка в индикаторе 5
#define L_BANK12  0x06 // Точка в индикаторах 1 и 2
#define L_Allon   0x07 // Точка во всех индикаторах
// H - 100% яркости индикаторов:
#define H_Alloff  0x08 // Все точки не горят
#define H_BANK1   0x09 // Точка в индикаторе 1
#define H_BANK2   0x0A // Точка в индикаторе 2
#define H_BANK3   0x0B // Точка в индикаторе 3
#define H_BANK4   0x0C // Точка в индикаторе 4
#define H_BANK5   0x0D // Точка в индикаторе 5
#define H_BANK12  0x0E // Точка в индикаторах 1 и 2
#define H_Allon   0x0F // Точка во всех индикаторах
//----------------------------------------

//-----------ПРОТОТИПЫ ФУНКЦИЙ------------
void WriteConfigMC14489(uint8_t config); // Настроить конфигурационный регистр
// Конфигурационный регистр состоит из 8 бит:
//   C7 C6 C5 C4 C3 C2 C1 C0
// 0b 1  1  0  0  0  0  0  1
//    |  |  /  /  /  /  /  +-определяет режим энергопотребления [1-нормальный; 0-низкое энергопотребление]
//    |  |  /  /  /  /  +=C1=[0-hex decode, 1-special decode/no decode (зависит от C6)]
//    |  |  /  /  /  +====C2=[0-hex decode, 1-special decode/no decode (зависит от C6)]
//    |  |  /  /  +=======C3=[0-hex decode, 1-special decode/no decode (зависит от C6)]
//    |  |  /  +==========C4=[0-hex decode, 1-special decode/no decode (зависит от C7)]
//    |  |  +=============C5=[0-hex decode, 1-special decode/no decode (зависит от C7)]
//    |  +-------------------[1-special/hex decode, 0-no decode для BANK C1,C2,C3]
//    +----------------------[1-special/hex decode, 0-no decode для BANK C4,C5]
void WriteSymbolsMC14489(uint8_t BANK1, uint8_t BANK2, uint8_t BANK3, uint8_t BANK4, uint8_t BANK5, uint8_t dt); // Вывод символов на индикаторы
void WriteDataMC14489(uint16_t dout, uint8_t dt); // Отправка числа [00000;65535] и точки
void WriteStringMC14489(const char *s, uint8_t dt); // Отправить строку данных и точку
void TxByteMC14489(uint8_t dout); // Отправка байта данных
//----------------------------------------

#endif /* MC14489_H_ */