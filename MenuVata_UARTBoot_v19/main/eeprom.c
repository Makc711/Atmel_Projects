#include "eeprom.h"

void EEPROM_write_byte(uint16_t uiAddress, uint8_t ucData){ // Записать 1 байт в EEPROM по адресу 
	while(EECR & (1<<EEWE)) // Ждем освобождения флага окончания последней операцией с памятью
	{}
	EEAR = uiAddress; // Устанавливаем адрес
	EEDR = ucData; // Записываем данные в регистр
	cli(); // Запрещаем глобальные прерывания
	EECR |= (1<<EEMWE); // Разрешаем делать запись в EEPROM
	EECR |= (1<<EEWE); // Пишем байт в память EEPROM
	sei(); // Разрешаем глобальные прерывания
}

uint8_t EEPROM_read_byte(uint16_t uiAddress){ // Прочитать 1 байт из EEPROM по адресу
	while(EECR & (1<<EEWE)) // Ждем освобождения флага окончания последней операцией с памятью
	{}
	EEAR = uiAddress; // Устанавливаем адрес
	EECR |= (1<<EERE); // Запускаем операцию считывания из памяти в регистр данных
	return EEDR; // Возвращаем результат
}

void EEPROM_write_word(uint16_t uiAddress, uint16_t ucData){ // Записать слово (2 байта) в EEPROM по адресу 
	EEPROM_write_byte(uiAddress, (uint8_t) ucData); // Запись младшего байта в EEPROM по адресу
	uint8_t dt = ucData>>8; // Сдвинем вправо на 8 бит
	EEPROM_write_byte(uiAddress + 1, dt); // Запись старшего байта в EEPROM по адресу
}

void EEPROM_write_dword(uint16_t uiAddress, uint32_t ucData){ // Записать двойное слово (4 байта) в EEPROM по адресу
	EEPROM_write_word(uiAddress, (uint16_t) ucData); // Запись младшего слова (2-х байт) в EEPROM по адресу
	uint16_t dt = ucData>>16; // Сдвинем вправо на 16 бит
	EEPROM_write_word(uiAddress + 2, dt); // Запись старшего слова (2-х байт) в EEPROM по адресу
}

uint16_t EEPROM_read_word(uint16_t uiAddress){ // Прочитать слово (2 байта) из EEPROM по адресу
	uint16_t dt = EEPROM_read_byte(uiAddress + 1) * 256; // Запись старшего байта в переменную dt
	asm("nop"); // На всякий случай (чтобы микроконтроллер успел считать данные)
	dt += EEPROM_read_byte(uiAddress); // Запись младшего байта в переменную dt
	return dt; // Возвратим результат
}

uint32_t EEPROM_read_dword(uint16_t uiAddress){ // Прочитать двойное слово (4 байта) из EEPROM по адресу
	uint32_t dt = EEPROM_read_word(uiAddress + 2) * 65536; // Запись старшего слова в переменную dt
	asm("nop"); // На всякий случай (чтобы микроконтроллер успел считать данные)
	dt += EEPROM_read_word(uiAddress); // Запись младшего слова в переменную dt
	return dt; // Возвратим результат
}

void EEPROM_write_msg(uint16_t uiAddress, const uint8_t *s, uint8_t strSize){ // Записать сообщение размером strSize байт в EEPROM по адресу
	for (uint8_t i=0; i<strSize; i++){ // Пишем все байты сообщения
		EEPROM_write_byte(uiAddress++, s[i]); // Записываем байт в EEPROM
	}
}

void EEPROM_write_string(uint16_t uiAddress, const char *s){ // Записать строку в EEPROM по адресу
	register char c; // Байт массива (из строки)
	while ( (c = *s++) ) // Пока в переменной "c" есть символ
		EEPROM_write_byte(uiAddress++, c); // Записываем байт в EEPROM
}

const char* EEPROM_read_string(uint16_t uiAddress, uint16_t sz){ // Прочитать строку из EEPROM по адресу uiAddress размером sz байт
	char *str1; // Создаем указатель на массив неопределенной длины
	str1 = (char *) realloc(NULL, sz); // Выделим область памяти под массив
	for (uint16_t i=0; i<sz; i++)
		str1[i] = EEPROM_read_byte(uiAddress + i); // Прочитаем в ячейку памяти массива байт из EEPROM
	return str1; // Возвратим результат
}