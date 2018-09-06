#include "eepromext.h"

uint8_t eepBuf[TWI_PAGE_SIZE + 3]; // Буффер для записи/чтения пакета данных (3 байта: 1 на адрес устройства + 2 на адрес ячейки памяти)
uint8_t eepState = 0; // Статус модуля TWI
extern uint8_t writeTime; // Таймер (нужен для отсчета времени между циклами записи)

uint8_t EE_Write_byte(uint16_t uiAddress, uint8_t ucData){ // Записать 1 байт во внешнюю EEPROM по адресу
	for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся записать байт повторно при возникновении ошибки
		while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
			_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
		eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
		eepBuf[1] = uiAddress>>8; // Старшая часть адреса ячейки памяти
		eepBuf[2] = (uint8_t) uiAddress; // Младшая часть адреса ячейки памяти
		eepBuf[3] = ucData; // Байт данных
		TWI_SendData(eepBuf, 4); // Передать сообщение eepBuf размером 4 байта по шине I2C
		eepState = TWI_GetState(); // Взять статус TWI модуля
		if (eepState == TWI_SUCCESS){ // Если не было ошибок
			return eepState; // Возвратим код статуса
		}
	}
	return eepState; // Возвратим код статуса
}

uint8_t EE_Write_word(uint16_t uiAddress, uint16_t ucData){ // Записать слово (2 байта) во внешнюю EEPROM по адресу
	for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся записать байт повторно при возникновении ошибки
		while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
			_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
		eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
		eepBuf[1] = uiAddress>>8; // Старшая часть адреса ячейки памяти
		eepBuf[2] = (uint8_t) uiAddress; // Младшая часть адреса ячейки памяти
		eepBuf[3] = (uint8_t) ucData; // Младший байт слова
		eepBuf[4] = (uint8_t) (ucData>>8); // Старший байт слова
		TWI_SendData(eepBuf, 5); // Передать сообщение eepBuf размером 5 байт по шине I2C
		eepState = TWI_GetState(); // Взять статус TWI модуля
		if (eepState == TWI_SUCCESS){ // Если не было ошибок
			return eepState; // Возвратим код статуса
		}
	}
	return eepState; // Возвратим код статуса
}

uint8_t EE_Write_dword(uint16_t uiAddress, uint32_t ucData){ // Записать двойное слово (4 байта) во внешнюю EEPROM по адресу
	for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся записать байт повторно при возникновении ошибки
		while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
			_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
		eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
		eepBuf[1] = uiAddress>>8; // Старшая часть адреса ячейки памяти
		eepBuf[2] = (uint8_t) uiAddress; // Младшая часть адреса ячейки памяти
		eepBuf[3] = (uint8_t) ucData; // Самый младший байт двойного слова
		eepBuf[4] = (uint8_t) (ucData>>8); // Байт двойного слова постарше
		eepBuf[5] = (uint8_t) (ucData>>16); // Байт двойного слова еще старше
		eepBuf[6] = (uint8_t) (ucData>>24); // Самый старший байт двойного слова
		TWI_SendData(eepBuf, 7); // Передать сообщение eepBuf размером 7 байт по шине I2C
		eepState = TWI_GetState(); // Взять статус TWI модуля
		if (eepState == TWI_SUCCESS){ // Если не было ошибок
			return eepState; // Возвратим код статуса
		}
	}
	return eepState; // Возвратим код статуса
}

uint8_t EE_Read_byte(uint16_t uiAddress, uint8_t* ucData){ // Прочитать 1 байт из внешней EEPROM по адресу
	while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
		_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
	for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся прочитать байт повторно при возникновении ошибки чтения
		eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
		eepBuf[1] = uiAddress>>8; // Старшая часть адреса ячейки памяти
		eepBuf[2] = (uint8_t) uiAddress; // Младшая часть адреса ячейки памяти
		TWI_SendData(eepBuf, 3); // Передаем адрес+W и адрес регистра (3 байта)
		eepBuf[0] = EEPROM_ADDR | (1<<TWI_READ_BIT); // Адрес устройства и последний бит = 1 (идет чтение из устройства)
		TWI_SendData(eepBuf, 1+1); // Передаем адрес+R и указываем, что будем принимать 1 байт
		eepState = TWI_GetData(eepBuf, 1+1); // Принимаем 1 байт в буффер eepBuf и записываем статус модуля TWI в переменную
		if (eepState == TWI_SUCCESS){ // Если не было ошибок
			*ucData = eepBuf[1];
			return eepState; // Возвратим код статуса
		}
	}
	*ucData = 0xFF; // Если была ошибка чтения, возвратим пустой байт
	return eepState; // Возвратим код статуса
}

uint8_t EE_Read_word(uint16_t uiAddress, uint16_t* ucData){ // Прочитать слово (2 байта) из EEPROM по адресу
	while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
		_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
	for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся прочитать слово повторно при возникновении ошибки чтения
		eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
		eepBuf[1] = uiAddress>>8; // Старшая часть адреса ячейки памяти
		eepBuf[2] = (uint8_t) uiAddress; // Младшая часть адреса ячейки памяти
		TWI_SendData(eepBuf, 3); // Передаем адрес+W и адрес регистра (3 байта)
		eepBuf[0] = EEPROM_ADDR | (1<<TWI_READ_BIT); // Адрес устройства и последний бит = 1 (идет чтение из устройства)
		TWI_SendData(eepBuf, 2+1); // Передаем адрес+R и указываем, что будем принимать 2 байта
		eepState = TWI_GetData(eepBuf, 2+1); // Принимаем 2 байта в буффер eepBuf и записываем статус модуля TWI в переменную
		if (eepState == TWI_SUCCESS){ // Если не было ошибок
			*ucData = (uint16_t) eepBuf[2] * 0x100 | (uint16_t) eepBuf[1]; // Возвратим считанное слово
			return eepState; // Возвратим код статуса
		}
	}
	*ucData = 0xFFFF; // Если была ошибка чтения, возвратим пустое слово
	return eepState; // Возвратим код статуса
}

uint8_t EE_Read_dword(uint16_t uiAddress, uint32_t* ucData){ // Прочитать двойное слово (4 байта) из EEPROM по адресу
	while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
		_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
	for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся прочитать двойное слово повторно при возникновении ошибки чтения
		eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
		eepBuf[1] = uiAddress>>8; // Старшая часть адреса ячейки памяти
		eepBuf[2] = (uint8_t) uiAddress; // Младшая часть адреса ячейки памяти
		TWI_SendData(eepBuf, 3); // Передаем адрес+W и адрес регистра (3 байта)
		eepBuf[0] = EEPROM_ADDR | (1<<TWI_READ_BIT); // Адрес устройства и последний бит = 1 (идет чтение из устройства)
		TWI_SendData(eepBuf, 4+1); // Передаем адрес+R и указываем, что будем принимать 4 байта
		eepState = TWI_GetData(eepBuf, 4+1); // Принимаем 4 байта в буффер eepBuf и записываем статус модуля TWI в переменную
		if (eepState == TWI_SUCCESS){ // Если не было ошибок
			*ucData = (uint32_t) eepBuf[4] * 0x1000000 | (uint32_t) eepBuf[3] * 0x10000 | (uint32_t) eepBuf[2] * 0x100 | (uint32_t) eepBuf[1];
			return eepState; // Возвратим код статуса
		}
	}
	*ucData = 0xFFFFFFFF; // Если была ошибка чтения, возвратим пустое двойное слово
	return eepState; // Возвратим код статуса
}

uint8_t EE_Write_msg(uint16_t uiAddress, const uint8_t *s, uint16_t strSize){ // Записать строку в EEPROM по адресу
	uint16_t pages = (strSize - 1) / TWI_PAGE_SIZE; // Количество целых страниц в передаваемом сообщении
	uint8_t pageSize = strSize - pages * TWI_PAGE_SIZE; // Количество байт на последней передаваемой странице
	for (uint16_t j=0; j<pages; j++){ // Выполняем для каждой передаваемой страницы
		for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся записать страницу повторно при возникновении ошибки
			while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
				_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
			eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
			eepBuf[1] = (uiAddress + j*TWI_PAGE_SIZE)>>8; // Старшая часть адреса ячейки памяти
			eepBuf[2] = (uint8_t) (uiAddress + j*TWI_PAGE_SIZE); // Младшая часть адреса ячейки памяти
			for (uint8_t i=0; i<TWI_PAGE_SIZE; i++){
				eepBuf[i+3] = s[i + j*TWI_PAGE_SIZE]; // Переписываем байты сообщения в буффер
			}
			TWI_SendData(eepBuf, TWI_PAGE_SIZE+3); // Передать сообщение eepBuf размером TWI_PAGE_SIZE+3 байт (целая страница + 3 байта адреса) по шине I2C
			eepState = TWI_GetState(); // Взять статус TWI модуля
			if (eepState == TWI_SUCCESS){ // Если не было ошибок
				break; // Выходим из цикла for
			}
		}
		if (eepState != TWI_SUCCESS){ // Если была ошибка записи
			return eepState; // Возвратим код статуса
		}
	}
	for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся записать байт повторно при возникновении ошибки
		while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
			_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
		eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
		eepBuf[1] = (uiAddress + pages*TWI_PAGE_SIZE)>>8; // Старшая часть адреса ячейки памяти
		eepBuf[2] = (uint8_t) (uiAddress + pages*TWI_PAGE_SIZE); // Младшая часть адреса ячейки памяти
		for (uint8_t i=0; i<pageSize; i++){
			eepBuf[i+3] = s[i + pages*TWI_PAGE_SIZE]; // Переписываем байты последней страницы сообщения в буффер
		}
		TWI_SendData(eepBuf, pageSize+3); // Передать сообщение eepBuf размером pageSize+3 байт (количество байт на последней странице сообщения + 3 байта адреса) по шине I2C
		eepState = TWI_GetState(); // Взять статус TWI модуля
		if (eepState == TWI_SUCCESS){ // Если не было ошибок
			break; // Выходим из цикла for
		}
	}
	return eepState; // Возвратим код статуса
}

uint8_t EE_Read_msg(uint16_t uiAddress, uint8_t *eepstr, uint16_t strSize){ // Прочитать строку из EEPROM по адресу uiAddress размером strSize байт
	uint16_t pages = (strSize-1)/TWI_PAGE_SIZE; // Количество целых страниц в принимаемом сообщении
	uint8_t pageSize = strSize - pages*TWI_PAGE_SIZE; // Количество байт на последней принимаемой странице
	while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
		_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
	for (uint16_t j=0; j<pages; j++){ // Выполняем для каждой принимаемой страницы
		for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся прочитать массив (строку) повторно при возникновении ошибки чтения
			eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
			eepBuf[1] = (uiAddress + j*TWI_PAGE_SIZE)>>8; // Старшая часть адреса ячейки памяти
			eepBuf[2] = (uint8_t) (uiAddress + j*TWI_PAGE_SIZE); // Младшая часть адреса ячейки памяти
			TWI_SendData(eepBuf, 3); // Передаем адрес+W и адрес регистра (3 байта)
			eepBuf[0] = EEPROM_ADDR | (1<<TWI_READ_BIT); // Адрес устройства и последний бит = 1 (идет чтение из устройства)
			TWI_SendData(eepBuf, TWI_PAGE_SIZE+1); // Передаем адрес+R и указываем, что будем принимать TWI_PAGE_SIZE байт (целую страницу)
			eepState = TWI_GetData(eepBuf, TWI_PAGE_SIZE+1); // Принимаем strSize байт в буффер eepBuf и записываем статус модуля TWI в переменную
			if (eepState == TWI_SUCCESS){ // Если не было ошибок
				for (uint8_t i=0; i<TWI_PAGE_SIZE; i++){ // Переписываем принятую страницу в массив
					eepstr[i + j*TWI_PAGE_SIZE] = eepBuf[i+1];
				}
				break; // Выходим из цикла for
			}
		}
		if (eepState != TWI_SUCCESS){ // Если была ошибка чтения
			memset(eepstr, 0xFF, strSize); // Обнуляем массив
			return eepState; // Возвратим код статуса
		}
	}
	for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся прочитать массив (строку) повторно при возникновении ошибки чтения
		eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
		eepBuf[1] = (uiAddress + pages*TWI_PAGE_SIZE)>>8; // Старшая часть адреса ячейки памяти
		eepBuf[2] = (uint8_t) (uiAddress + pages*TWI_PAGE_SIZE); // Младшая часть адреса ячейки памяти
		TWI_SendData(eepBuf, 3); // Передаем адрес+W и адрес регистра (3 байта)
		eepBuf[0] = EEPROM_ADDR | (1<<TWI_READ_BIT); // Адрес устройства и последний бит = 1 (идет чтение из устройства)
		TWI_SendData(eepBuf, pageSize+1); // Передаем адрес+R и указываем, что будем принимать pageSize байт
		eepState = TWI_GetData(eepBuf, pageSize+1); // Принимаем pageSize байт в буффер eepBuf и записываем статус модуля TWI в переменную
		if (eepState == TWI_SUCCESS){ // Если не было ошибок
			for (uint8_t i=0; i<pageSize; i++){ // Переписываем принятую страницу в массив
				eepstr[i + pages*TWI_PAGE_SIZE] = eepBuf[i+1];
			}
			break; // Выходим из цикла for
		}
	}
	if (eepState == TWI_SUCCESS){ // Если не было ошибок чтения
		return eepState; // Возвратим код статуса
	}
	else {
		memset(eepstr, 0xFF, strSize); // Обнуляем массив
		return eepState; // Возвратим код статуса
	}
}

uint8_t EE_Clear_all_memory(uint8_t empty){ // Очистить всю внешнюю EEPROM память
	uint16_t pages = MEMORY/TWI_PAGE_SIZE; // Количество целых страниц в передаваемом сообщении
	memset(eepBuf, empty, TWI_PAGE_SIZE+3); // Обнуляем массив
	for (uint16_t j=0; j<pages; j++){ // Выполняем для каждой передаваемой страницы
		for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся записать байт повторно при возникновении ошибки
			while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
				_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
			eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
			eepBuf[1] = (j*TWI_PAGE_SIZE)>>8; // Старшая часть адреса ячейки памяти
			eepBuf[2] = (uint8_t) (j*TWI_PAGE_SIZE); // Младшая часть адреса ячейки памяти
			TWI_SendData(eepBuf, TWI_PAGE_SIZE+3); // Передать сообщение eepBuf размером TWI_PAGE_SIZE+3 байт (целая страница + 3 байта адреса) по шине I2C
			eepState = TWI_GetState(); // Взять статус TWI модуля
			if (eepState == TWI_SUCCESS){ // Если не было ошибок
				break; // Выходим из цикла for
			}
		}
		if (eepState != TWI_SUCCESS){ // Если была ошибка записи
			return eepState; // Возвратим код статуса
		}
	}
	return eepState; // Возвратим код статуса
}

uint8_t EE_Read_all_memory(void){ // Прочитать всю внешнюю память EEPROM и отправить байты данных по USART
	uint16_t pages = MEMORY/TWI_PAGE_SIZE; // Количество целых страниц в принимаемом сообщении
	while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
		_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
	for (uint16_t j=0; j<pages; j++){ // Выполняем для каждой принимаемой страницы
		for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся прочитать массив (строку) повторно при возникновении ошибки чтения
			eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
			eepBuf[1] = (j*TWI_PAGE_SIZE)>>8; // Старшая часть адреса ячейки памяти
			eepBuf[2] = (uint8_t) (j*TWI_PAGE_SIZE); // Младшая часть адреса ячейки памяти
			TWI_SendData(eepBuf, 3); // Передаем адрес+W и адрес регистра (3 байта)
			eepBuf[0] = EEPROM_ADDR | (1<<TWI_READ_BIT); // Адрес устройства и последний бит = 1 (идет чтение из устройства)
			TWI_SendData(eepBuf, TWI_PAGE_SIZE+1); // Передаем адрес+R и указываем, что будем принимать TWI_PAGE_SIZE байт (целую страницу)
			eepState = TWI_GetData(eepBuf, TWI_PAGE_SIZE+1); // Принимаем strSize байт в буффер eepBuf и записываем статус модуля TWI в переменную
			if (eepState == TWI_SUCCESS){ // Если не было ошибок
				break; // Выходим из цикла for
			}
		}
		if (eepState == TWI_SUCCESS){ // Если не было ошибок чтения
			for (uint8_t i=1; i<TWI_PAGE_SIZE+1; i++){ // Отправляем принятую страницу по USART
//				USART_Transmit_byte(eepBuf[i]); // Отправить 1 байт данных по USART
			}
		}
		else {
			memset(eepBuf, 0xFF, TWI_PAGE_SIZE); // Обнуляем массив
			for (uint8_t i=1; i<TWI_PAGE_SIZE+1; i++){ // Отправляем принятую страницу по USART
//				USART_Transmit_byte(eepBuf[i]); // Отправить 1 байт данных по USART
			}
			return eepState; // Возвратим код статуса
		}
	}
	return eepState; // Возвратим код статуса
}

uint8_t EE_check_memory(void){ // Проверка всех ячеек памяти. Возвращает количество битых ячеек памяти
	uint8_t badMemCell = 0; // Счетчик плохих ячеек памяти
	uint8_t twState = 0; // Rод статуса модуля TWI 
	uint16_t pages = MEMORY/TWI_PAGE_SIZE; // Количество целых страниц в принимаемом сообщении
	twState = EE_Clear_all_memory(0x55); // Записываем во все ячейки памяти байт 0x55
	if (twState != TWI_SUCCESS){
		badMemCell = 0xFF;
		return badMemCell; // Возвратим количество плохих ячеек памяти
	}
	while(writeTime) // Внешняя EEPROM пока еще не готова для работы (записывается предыдущая страница)
		_delay_us(1); // Ждем, пока предыдущая страница запишется во внешнюю EEPROM
	for (uint16_t j=0; j<pages; j++){ // Выполняем для каждой принимаемой страницы
		for (uint8_t i=0; i<ATTEMPT; i++){ // Пытаемся прочитать массив (строку) повторно при возникновении ошибки чтения
			eepBuf[0] = EEPROM_ADDR; // Адрес устройства и последний бит = 0 (идет запись в устройство)
			eepBuf[1] = (j*TWI_PAGE_SIZE)>>8; // Старшая часть адреса ячейки памяти
			eepBuf[2] = (uint8_t) (j*TWI_PAGE_SIZE); // Младшая часть адреса ячейки памяти
			TWI_SendData(eepBuf, 3); // Передаем адрес+W и адрес регистра (3 байта)
			eepBuf[0] = EEPROM_ADDR | (1<<TWI_READ_BIT); // Адрес устройства и последний бит = 1 (идет чтение из устройства)
			TWI_SendData(eepBuf, TWI_PAGE_SIZE+1); // Передаем адрес+R и указываем, что будем принимать TWI_PAGE_SIZE байт (целую страницу)
			eepState = TWI_GetData(eepBuf, TWI_PAGE_SIZE+1); // Принимаем strSize байт в буффер eepBuf и записываем статус модуля TWI в переменную
			if (eepState == TWI_SUCCESS){ // Если не было ошибок
				break; // Выходим из цикла for
			}
		}
		if (eepState == TWI_SUCCESS){ // Если не было ошибок чтения
			for (uint8_t i=1; i<TWI_PAGE_SIZE+1; i++){ // Обрабатываем все байты страницы
				if (eepBuf[i] != 0x55) // Если прочитанный байт не равен записанному в эту ячейку памяти
					badMemCell++; // Увеличиваем счетчик плохих ячеек памяти
			}
		}
		else {
			badMemCell = 0xFF;
			return badMemCell; // Возвратим количество плохих ячеек памяти
		}
	}
	return badMemCell; // Возвратим количество плохих ячеек памяти
}