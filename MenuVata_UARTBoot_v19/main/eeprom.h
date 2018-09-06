#ifndef EEPROM_H_
#define EEPROM_H_

#include "main.h"

void EEPROM_write_byte(uint16_t uiAddress, uint8_t ucData); // Записать 1 байт в EEPROM по адресу 
void EEPROM_write_word(uint16_t uiAddress, uint16_t ucData); // Записать слово (2 байта) в EEPROM по адресу 
void EEPROM_write_dword(uint16_t uiAddress, uint32_t ucData); // Записать двойное слово (4 байта) в EEPROM по адресу
void EEPROM_write_msg(uint16_t uiAddress, const uint8_t *s, uint8_t strSize); // Записать сообщение размером strSize байт в EEPROM по адресу
void EEPROM_write_string(uint16_t uiAddress, const char *s); // Записать строку в EEPROM по адресу
uint8_t EEPROM_read_byte(uint16_t uiAddress); // Прочитать 1 байт из EEPROM по адресу
uint16_t EEPROM_read_word(uint16_t uiAddress); // Прочитать слово (2 байта) из EEPROM по адресу
uint32_t EEPROM_read_dword(uint16_t uiAddress); // Прочитать двойное слово (4 байта) из EEPROM по адресу
const char* EEPROM_read_string(uint16_t uiAddress, uint16_t sz); // Прочитать строку из EEPROM по адресу uiAddress размером sz байт

#endif /* EEPROM_H_ */