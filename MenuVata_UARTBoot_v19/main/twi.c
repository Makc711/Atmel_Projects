#include "twi.h"

#define TWSR_MASK     0xF8 // Маска для кодов состояния

#ifndef TWI_PAGE_SIZE // Если размер страницы не определен
	#  warning "TWI_PAGE_SIZE is not defined"
	#define TWI_PAGE_SIZE 8 // Задать его равным 8 байт
#endif

#ifndef WRITE_CYCLE // Если задержка между циклами записи не определена
	#  warning "WRITE_CYCLE is not defined"
	#define WRITE_CYCLE 10 // Задать ее равной 10 мс
#endif


volatile static uint8_t twiBuf[TWI_PAGE_SIZE+3]; // Буффер для записи/чтения пакета данных + 3 байта адресов
volatile static uint8_t twiState = TWI_NO_STATE; // Статус модуля TWI
volatile static uint8_t twiMsgSize; // Количество байтов передаваемых/принимаемых в одном сообщении
uint8_t writeTime = 0; // Таймер (нужен для отсчета времени между циклами записи)

INIT(7){ // Инициализация шины I2C
	TWBR = (uint8_t)TWBR_VALUE; // Скорость связи
	TWSR = TWPS; // Задаем предделитель
	TWDR = 0xFF; // Регистр данных шины TWI
	TWCR = (1<<TWEN); // Разрешаем работу TWI
}

static uint8_t TWI_TransceiverBusy(void){ // Проверка - не занят ли TWI модуль
	return (TWCR & (1<<TWIE));                 
}

 uint8_t TWI_GetState(void){ // Взять статус TWI модуля
	while(TWI_TransceiverBusy());
	return twiState;
 }

void TWI_SendData(uint8_t *msg, uint8_t msgSize){ // Передать сообщение msg размером msgSize байтов по шине I2C
	while(TWI_TransceiverBusy()); // Ждем, когда TWI модуль освободится
	
	twiMsgSize = msgSize; // Сохряняем кол. байт для передачи             
	twiBuf[0]  = msg[0];  // и первый байт сообщения 
	
	if (!(msg[0] & (1<<TWI_READ_BIT))){ // Если первый байт типа SLA+W
		for (uint8_t i = 1; i < msgSize; i++){ // то сохряняем остальную часть сообщения
			twiBuf[i] = msg[i];
		}
	}
                       
	twiState = TWI_NO_STATE ;
	TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); // Разрешаем прерывание и формируем состояние СТАРТ
}

uint8_t TWI_GetData(uint8_t *msg, uint8_t msgSize){ // Переписать полученные данные в буфер msg в количестве msgSize байт
	while(TWI_TransceiverBusy()); // Ждем, когда TWI модуль освободится
	
	if (twiState == TWI_SUCCESS){ // Если сообщение успешно принято,                         
		for (uint8_t i = 0; i < msgSize; i++){ // то переписываем его из внутреннего буфера в переданный
			msg[i] = twiBuf[i];
		}
	}
	
	return twiState;                                   
}

ISR(TWI_vect){ // Обработчик прерывания TWI модуля
	static uint8_t ptr; // Счетчик байт в передаваемом/принимаемом сообщении
	uint8_t stat = TWSR & TWSR_MASK; // Регистр статуса
	
	switch (stat){ // Проверяем статус
    
		case TWI_START:					 // Состояние СТАРТ сформировано 
		case TWI_REP_START:				 // Состояние ПОВТОРНЫЙ СТАРТ сформировано (повтор условия начала передачи)   
			ptr = 0; // Начинаем работу с первого байта массива

		case TWI_MT_SLA_ACK:             // Ведущий отправил адрес и бит записи. Ведомый подтвердил свой адрес
		case TWI_MT_DATA_ACK:            // Ведущий передал данные и ведомый подтвердил прием
			if (ptr < twiMsgSize){ // Если переданы не все байты сообщения
				TWDR = twiBuf[ptr]; // Загружаем в регистр данных следующий байт
				TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT); // Сбрасываем флаг TWINT    
				ptr++; // Переходим к следующему байту сообщения
			}
			else { // Если все байты сообщения переданы
				twiState = TWI_SUCCESS; // Передача сообщения произошла успешно
				TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO)|(0<<TWIE); // Формируем состояние СТОП, сбрасываем флаг, запрещаем прерывания
				writeTime = TICK0_WRITE_CYCLE; // Устанавливаем таймер на WRITE_CYCLE мс
			}
			break;
     
		case TWI_MR_DATA_ACK:            // Ведущий принял данные и передал подтверждение 
			twiBuf[ptr] = TWDR; // Записываем в буффер принятый байт
			ptr++; // Переходим к следующему байту сообщения
    
		case TWI_MR_SLA_ACK:             // Ведущий отправил адрес и бит чтения. Ведомый подтвердил свой адрес
			if (ptr < (twiMsgSize-1)){ // Если это НЕ предпоследний принятый байт
				TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA); // Формируем подтверждение                             
			}
			else { // Если приняли предпоследний байт
				TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT); // Подтверждение НЕ формируем
			}    
			break; 
      
		case TWI_MR_DATA_NACK:			 // Ведущий принял данные и НЕ передал подтверждение    
			twiBuf[ptr] = TWDR; // Записываем в буффер принятый байт
			twiState = TWI_SUCCESS; // Все байты сообщения были приняты
			TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO); // Формируем состояние СТОП
			break; 
     
		case TWI_ARB_LOST:				 // Был потерян приоритет 
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); // Сбрасываем флаг TWINT, формируем повторный СТАРТ
			break;
      
		case TWI_MT_SLA_NACK:			 // Ведущий отправил адрес и бит записи. Нет подтверждения приема (ведомый с таким адресом не найден)
		case TWI_MR_SLA_NACK:			 // Ведущий отправил адрес и бит чтения. Нет подтверждения приема (ведомый с таким адресом не найден)
		case TWI_MT_DATA_NACK:			 // Ведущий передал данные. Нет подтверждения приема
		case TWI_BUS_ERROR:				 // Ошибка на шине из-за некоректных состояний СТАРТ или СТОП
		default:     
			twiState = stat; // Записываем статус ошибки
			TWCR = (1<<TWEN)|(0<<TWIE)|(0<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC); // Запретить прерывание                              
	}
}
