#ifndef EEPROMEXT_H_
#define EEPROMEXT_H_

#include "main.h"


#define AT24C64 // Название микросхемы памяти
#define EEPROM_ADDR 0b10100000 // Адрес устройства (состоит из 7 бит + восьмой бит (бит квитирования - R/W))
// Младший (восьмой) бит адреса всегда равен 0! (это  бит квитирования, он =0, когда идет запись и =1, когда чтение)
#define WRITE_CYCLE     10     // Задержка между циклами записи, мс
#define ATTEMPT     3    // Количество попыток чтения/записи при возникновении ошибки чтения/записи

//***********************************************************
// АВТОНАСТРОЙКА ПАМЯТИ
//-----------------------------------------------------------
#ifdef AT24C01
	#define MEMORY          128    // Объем памяти, байт
	#define TWI_PAGE_SIZE   8      // Размер страницы, байт
#endif
#ifdef AT24C02
	#define MEMORY          256    // Объем памяти, байт
	#define TWI_PAGE_SIZE   8      // Размер страницы, байт
#endif
#ifdef AT24C04
	#define MEMORY          512    // Объем памяти, байт
	#define TWI_PAGE_SIZE   16     // Размер страницы, байт
#endif
#ifdef AT24C08
	#define MEMORY          1024   // Объем памяти, байт
	#define TWI_PAGE_SIZE   16     // Размер страницы, байт
#endif
#ifdef AT24C16
	#define MEMORY          2048   // Объем памяти, байт
	#define TWI_PAGE_SIZE   16     // Размер страницы, байт
#endif
#ifdef AT24C32
	#define MEMORY          4096   // Объем памяти, байт
	#define TWI_PAGE_SIZE   32     // Размер страницы, байт
#endif
#ifdef AT24C64
	#define MEMORY          8192   // Объем памяти, байт
	#define TWI_PAGE_SIZE   32     // Размер страницы, байт
#endif
#ifdef AT24C128
	#define MEMORY          16384  // Объем памяти, байт
	#define TWI_PAGE_SIZE   64     // Размер страницы, байт
#endif
#ifdef AT24C256
	#define MEMORY          32768  // Объем памяти, байт
	#define TWI_PAGE_SIZE   64     // Размер страницы, байт
#endif
#ifdef AT24C512
	#define MEMORY          65536  // Объем памяти, байт
	#define TWI_PAGE_SIZE   128    // Размер страницы, байт
#endif
#ifdef AT24C1024
	#define MEMORY          131072 // Объем памяти, байт
	#define TWI_PAGE_SIZE   256    // Размер страницы, байт
#endif
#ifdef BR24C64
	#define MEMORY          8192   // Объем памяти, байт
	#define TWI_PAGE_SIZE   64     // Размер страницы, байт
#endif
//***********************************************************
// НЕ ВЫЗЫВАТЬ ФУНКЦИИ В ОБРАБОТЧИКЕ ПРЕРЫВАНИЙ!!!!!
uint8_t EE_Write_byte(uint16_t uiAddress, uint8_t ucData); // Записать 1 байт во внешнюю EEPROM по адресу
uint8_t EE_Write_word(uint16_t uiAddress, uint16_t ucData); // Записать слово (2 байта) в EEPROM по адресу 
uint8_t EE_Write_dword(uint16_t uiAddress, uint32_t ucData); // Записать двойное слово (4 байта) во внешнюю EEPROM по адресу
uint8_t EE_Write_msg(uint16_t uiAddress, const uint8_t *s, uint16_t strSize); // Записать сообщение s размером strSize байт во внешнюю EEPROM по адресу uiAddress

uint8_t EE_Read_byte(uint16_t uiAddress, uint8_t* ucData); // Прочитать 1 байт из внешней EEPROM по адресу
uint8_t EE_Read_word(uint16_t uiAddress, uint16_t* ucData); // Прочитать слово (2 байта) из внешней EEPROM по адресу
uint8_t EE_Read_dword(uint16_t uiAddress, uint32_t* ucData); // Прочитать двойное слово (4 байта) из внешней EEPROM по адресу
uint8_t EE_Read_msg(uint16_t uiAddress, uint8_t *eepstr, uint16_t strSize); // Прочитать сообщение из внешней EEPROM по адресу uiAddress размером strSize байт
uint8_t EE_Clear_all_memory(uint8_t empty); // Очистить всю внешнюю EEPROM память, empty - код пустой ячейки памяти (0xFF)
uint8_t EE_Read_all_memory(void); // Прочитать всю внешнюю память EEPROM и отправить байты данных по USART
uint8_t EE_check_memory(void); // Проверка всех ячеек памяти. Возвращает количество битых ячеек памяти

#endif /* EEPROMEXT_H_ */