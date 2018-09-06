#include "DS1307.h"

extern uint8_t mday, // День месяца [1;31]
			   month, // Месяц [1;12]
			   year, // Год [0;99]
			   wday, // День недели [1;7]
			   hour, // Час [0;24]
			   minute, // Минута [0;59]
			   second; // Секунда [0;59]
uint8_t clockBuf[10]; // Буффер для записи/чтения данных

static uint8_t Convert_BinDec_to_Dec(uint8_t c){ // Перевод двоично-десятичного числа в десятичное
	uint8_t ch = ((c>>4) * 10 + (0b00001111 & c)); // Десятки + единицы
	return ch;
}

static uint8_t Convert_Dec_to_BinDec(uint8_t c){ // Перевод десятичного числа в двоично-десятичное
	uint8_t ch = ((c / 10)<<4) | (c % 10); // Десятки + единицы
	return ch;
}

void DS1307_Set_Time(void){ // Установить время (Запуск часов)
	clockBuf[0] = DS1307_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
	clockBuf[1] = 0; // Переходим на адрес памяти 0x00 (0x00-0x06 = second|minute|hour|wday|mday|month|year)
	clockBuf[2] = Convert_Dec_to_BinDec(second); // Cекунды
	clockBuf[3] = Convert_Dec_to_BinDec(minute); // Минуты
	clockBuf[4] = Convert_Dec_to_BinDec(hour); // Часы
	clockBuf[5] = Convert_Dec_to_BinDec(wday); // День недели
	clockBuf[6] = Convert_Dec_to_BinDec(mday); // День месяца
	clockBuf[7] = Convert_Dec_to_BinDec(month); // Месяц
	clockBuf[8] = Convert_Dec_to_BinDec(year); // Год
	clockBuf[9] = 0b00010000; // Настройка вывода SQW/OUT: SQWE=1 - включить генерацию импульсов на выводе SQW/OUT
	TWI_SendData(clockBuf, 10); // Передать сообщение clockBuf размером 10 байт по шине I2C
}

void DS1307_Read_Time(void){ // Прочитать время в переменные mday..second
	clockBuf[0] = DS1307_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
	clockBuf[1] = 0; // Переходим на адрес памяти 0x00 (0x00-0x06 = second|minute|hour|wday|mday|month|year)
	TWI_SendData(clockBuf, 2); // Передаем адрес+W и адрес регистра (2 байта)
	clockBuf[0] = DS1307_ADDR | (1<<TWI_READ_BIT); // Адрес устройства и последний бит = 1 (идет чтение из устройства)
	TWI_SendData(clockBuf, 7+1); // Передаем адрес+R и указываем, что будем принимать 7 байт
	TWI_GetData(clockBuf, 7+1); // Принимаем 7 байт в буффер clockBuf
	second = Convert_BinDec_to_Dec(clockBuf[1]); // Чтение байта данных, преобразование и запись в переменную
	minute = Convert_BinDec_to_Dec(clockBuf[2]); // Чтение байта данных, преобразование и запись в переменную
	hour = Convert_BinDec_to_Dec(clockBuf[3]); // Чтение байта данных, преобразование и запись в переменную
	wday = Convert_BinDec_to_Dec(clockBuf[4]); // Чтение байта данных, преобразование и запись в переменную
	mday = Convert_BinDec_to_Dec(clockBuf[5]); // Чтение байта данных, преобразование и запись в переменную
	month = Convert_BinDec_to_Dec(clockBuf[6]); // Чтение байта данных, преобразование и запись в переменную
	year = Convert_BinDec_to_Dec(clockBuf[7]); // Чтение последнего байта данных, преобразование и запись в переменную
}