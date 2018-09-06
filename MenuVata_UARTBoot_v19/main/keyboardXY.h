#ifndef keyboardXY_H_
#define keyboardXY_H_

#include "main.h"

//-------------------------------------//
//        Код нажатой кнопки           //
//-------------------------------------//
enum {
	Butt_Esc = 1,
	Butt_Left,
	Butt_Down,
	Butt_Up,
	Butt_Right,
	Butt_Select,
	Butt_Start,
	Butt_Change,
	Butt_Stop
};
//--------------------------------------

#define key_null 0x00     // кнопка не нажата
#define key11 Butt_Esc    // нажата кнопка ESC
#define key12 Butt_Left   // нажата кнопка ВЛЕВО
#define key13 Butt_Down   // нажата кнопка ВНИЗ
#define key21 Butt_Up     // нажата кнопка ВВЕРХ
#define key22 Butt_Right  // нажата кнопка ВПРАВО
#define key23 Butt_Select // нажата кнопка SELECT
#define key31 Butt_Stop   // нажата кнопка STOP
#define key32 Butt_Change // нажата кнопка T/RPM
#define key33 Butt_Start  // нажата кнопка START

// Если все линии управления на 1 порту - нужно описать его,
// иначе макрос закомментировать!
#define 	PORT_KEYBOARD		C

// Далее идет описание портов и пинов матричной клавиатуры.
// Номер пина - это номер бита в порту, т.е. число от 0 до 7
#define		PORT_stolb1			PORT_KEYBOARD // Порт столбца 1
#define		PIN_stolb1			5             // Пин столбца 1

#define		PORT_stolb2			PORT_KEYBOARD // Порт столбца 2
#define		PIN_stolb2			4             // Пин столбца 2

#define		PORT_stolb3			PORT_KEYBOARD // Порт столбца 3
#define		PIN_stolb3			3             // Пин столбца 3

#define		PORT_stroka1		PORT_KEYBOARD // Порт строки 1
#define		PIN_stroka1			2             // Пин строки 1

#define		PORT_stroka2		PORT_KEYBOARD // Порт строки 2
#define		PIN_stroka2			1             // Пин строки 2

#define		PORT_stroka3		PORT_KEYBOARD // Порт строки 3
#define		PIN_stroka3			0             // Пин строки 3

#define THRESHOLD 20 // Сколько циклов опроса кнопок кнопка должна удерживаться (защита от дребезга)
#define TIMEPRESS_SLOW 150 // Время между "медленными нажатиями" на кнопку при ее удержании [TIMEPRESS_FAST;255-THRESHOLD)
#define TIMEPRESS_FAST 50 // Время между "быстрыми нажатиями" на кнопку при ее удержании (10;TIMEPRESS_SLOW]
#define CLICKS 3 // Количество "медленных нажатий" на кнопку перед "быстрыми нажатиями"

#define PRESCALER_T0 64 // Предделитель таймера-счетчика 0
#define TICK_INT_T0 256 // Количество тиков Т0 до прерывания по переполнению (2^8 или 2^16)
#define TICKSEC (F_CPU/PRESCALER_T0/TICK_INT_T0) // Количество тиков Т0 проходит за 1 сек (период прерывания Т0)
#define HALFSEC (TICKSEC/2) // Количество тиков Т0 проходит за 500 мс
#define TICK750 ((TICKSEC*75/100)+1) // Количество тиков Т0 проходит за 750 мс (+1 - для того чтобы было не меньше 750 мс)
#define TICK0_WRITE_CYCLE ((TICKSEC*WRITE_CYCLE/1000) + 2) // Количество тиков Т0 по переполнению проходит за 10 мс (450*10/1000+1 = 5) (+2 - для того чтобы было не меньше 10 мс)

uint8_t KEYB_GetKey(void);       // Взять код нажатой кнопки

#endif //keyboardXY_H_