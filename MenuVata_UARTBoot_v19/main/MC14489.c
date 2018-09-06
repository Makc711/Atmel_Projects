#include "MC14489.h"

INIT(7){ // Инициализация драйвера 7-сегментных индикаторов
	DDR(PORT_DATAIN) |= _BV(PIN_DATAIN); // Установка линий PIN порта на выход
	DDR(PORT_CLOCK) |= _BV(PIN_CLOCK);
	DDR(PORT_ENABLE) |= _BV(PIN_ENABLE);
	// Изначальное состояние:
	_ENABLE_1; 
	_CLOCK_0;
	_DATAIN_1;
}

void WriteConfigMC14489(uint8_t config){ // Настроить конфигурационный регистр
	_ENABLE_0; // Начало передачи данных
	TxByteMC14489(config); // Отправка байта данных
	_ENABLE_1; // Конец передачи данных
}

void WriteSymbolsMC14489(uint8_t BANK1, uint8_t BANK2, uint8_t BANK3, uint8_t BANK4, uint8_t BANK5, uint8_t dt){ // Вывод символов на индикаторы
	uint8_t byte0, byte1, byte2; // Байты с символами для отправки на драйвер индикаторов
	byte0 = (dt<<4)|BANK5; // Формируем байт из двух полубайтовых символов
	byte1 = (BANK4<<4)|BANK3;
	byte2 = (BANK2<<4)|BANK1;
	_ENABLE_0; // Начало передачи данных
	TxByteMC14489(byte0); // Отправка байта данных
	TxByteMC14489(byte1);
	TxByteMC14489(byte2);
	_ENABLE_1; // Конец передачи данных
}

void WriteDataMC14489(uint16_t dout, uint8_t dt){ // Отправка числа [00000;65535] и точки
	WriteConfigMC14489(0b11000001); // Настроить конфигурационный регистр
	//                     54321
	//					   hhhhh
	uint8_t BANK[5];
	for (uint8_t n=5; n>0; n--){
		BANK[n] = dout%10;
		dout = dout/10;
	}
	WriteSymbolsMC14489(BANK[1], BANK[2], BANK[3], BANK[4], BANK[5], dt); // Вывод символов на индикаторы
}

void WriteStringMC14489(const char *s, uint8_t dt){ // Отправить строку данных и точку
	uint32_t BANKnumeric = BANKnum; // Порядок следования индикаторов
	uint8_t BANKnumber; // Номер индикатора
	uint8_t BANK[5] = {Sym__, Sym__, Sym__, Sym__, Sym__}; // Массив символов
	uint8_t f = 0; // Проверка на переполнение
	uint8_t Config = 0xFF; // Байт настройки конфигурационного регистра
	// Перекодировка символов из ASCII в HEX:
	while(*s && f<5){ // Пока есть символ в строке и их количество не больше 5
		f++; // Номер символа в строке
		BANKnumber = (BANKnumeric%10)-1;
		BANKnumeric = BANKnumeric/10;
		BANK[BANKnumber] = *s++; // Записать символ в ячейку массива и перейти к следующему символу
 		switch (BANK[BANKnumber]){ // Проверяем код символа кодировки ASCII
			case ASCII_Sym_0: // 0
			case ASCII_Sym_O: // O
				BANK[BANKnumber] = Sym_0;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_1: // 1
				BANK[BANKnumber] = Sym_1;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_2: // 2
				BANK[BANKnumber] = Sym_2;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_3: // 3
				BANK[BANKnumber] = Sym_3;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_4: // 4
				BANK[BANKnumber] = Sym_4;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_5: // 5
			case ASCII_Sym_S: // S
				BANK[BANKnumber] = Sym_5;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_6: // 6
				BANK[BANKnumber] = Sym_6;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_7: // 7
				BANK[BANKnumber] = Sym_7;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_8: // 8
			case ASCII_Sym_B: // B
				BANK[BANKnumber] = Sym_8;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_9: // 9
			case ASCII_Sym_g: // g
				BANK[BANKnumber] = Sym_9;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_A: // A
				BANK[BANKnumber] = Sym_A;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_b: // b
				BANK[BANKnumber] = Sym_b;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_C: // C
				BANK[BANKnumber] = Sym_C;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_d: // d
				BANK[BANKnumber] = Sym_d;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_E: // E
				BANK[BANKnumber] = Sym_E;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym_F: // F
				BANK[BANKnumber] = Sym_F;
				Config &= ~(1<<(BANKnumber+1)); // Hex Decode
				break;
			case ASCII_Sym__: // Пробел
				BANK[BANKnumber] = Sym__;
				break;
			case ASCII_Sym_c: // c
				BANK[BANKnumber] = Sym_c;
				break;
			case ASCII_Sym_H: // H
				BANK[BANKnumber] = Sym_H;
				break;
			case ASCII_Sym_h: // h
				BANK[BANKnumber] = Sym_h;
				break;
			case ASCII_Sym_J: // J
				BANK[BANKnumber] = Sym_J;
				break;
			case ASCII_Sym_L: // L
				BANK[BANKnumber] = Sym_L;
				break;
			case ASCII_Sym_n: // n
				BANK[BANKnumber] = Sym_n;
				break;
			case ASCII_Sym_o: // o
				BANK[BANKnumber] = Sym_o;
				break;
			case ASCII_Sym_P: // P
				BANK[BANKnumber] = Sym_P;
				break;
			case ASCII_Sym_r: // r
				BANK[BANKnumber] = Sym_r;
				break;
			case ASCII_Sym_U: // U
				BANK[BANKnumber] = Sym_U;
				break;
			case ASCII_Sym_u: // u
				BANK[BANKnumber] = Sym_u;
				break;
			case ASCII_Sym_Y: // Y
				BANK[BANKnumber] = Sym_Y;
				break;
			case ASCII_Sym_tire: // -
				BANK[BANKnumber] = Sym_tire;
				break;
			case ASCII_Sym_ravno: // =
				BANK[BANKnumber] = Sym_ravno;
				break;
			case ASCII_Sym_grad: // Символ '
				BANK[BANKnumber] = Sym_grad;
				break;
			default:
				BANK[BANKnumber] = Sym__; // Пробел
		}
	}
	WriteConfigMC14489(Config); // Настроить конфигурационный регистр
	WriteSymbolsMC14489(BANK[0], BANK[1], BANK[2], BANK[3], BANK[4], dt); // Вывод символов на индикаторы
}

void TxByteMC14489(uint8_t dout){ // Отправка байта данных
	for(uint8_t b=0; b<8; b++){ // 8 бит
		if(dout & 0x80)   _DATAIN_1; // Если в старшем байте бит равен 1, то DATAIN=1
		else              _DATAIN_0; // иначе DATAIN=0
		_CLOCK_1;
		dout <<= 1; // Сдвинуть байт на 1 бит влево
		_CLOCK_0;
	}
}