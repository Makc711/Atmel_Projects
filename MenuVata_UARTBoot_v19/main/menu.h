#ifndef MENU_H_
#define MENU_H_
//-----------------------------------------------------------
#include "main.h"
//-----------------------------------------------------------
// Настройка времени мигания пунктами меню
#define BURNING_TIME 80 // Время свечения при мигании (% от 1 сек.)
#define BURNING_TICK (TICKSEC*BURNING_TIME/100) // Время свечения при мигании (в тиках)
//-----------------------------------------------------------
#define RPM_SETMAX 2800 // Максимальная частота вращения, об/мин
#define RPM_SETMIN 200 // Минимальная частота вращения, об/мин
#define TEMPER_SETMAX 25000 // Максимальная температура нагрева, 'С/100
#define TEMPER_SETMIN 2000 // Минимальная температура нагрева, 'С/100
//-----------------------------------------------------------
enum { 
	COPY,  // Производим копирование памяти
	ERASE // Производим стирание памяти
};
// Направления движения по меню
enum { // Структура с перечислением для указания направления движения по меню
	DIRECTION_NO, // Нет направления перехода
	DIRECTION_UP,   // Снизу вверх
	DIRECTION_DOWN, // Сверху вниз
	DIRECTION_LEFT, // Слева налево
	DIRECTION_RIGHT // Справа направо
};
//-----------------------------------------------------------
// NAME_IDLE - вывод пункта меню (один раз)
// NAME_WAIT - ожидаем нажатия кнопки (цикл)
// NAME      - изменяем значение
typedef enum { // Структура с перечислением для вызова меню
	MENU_STATE_IDLE, // Старт программы
	MENU_STATE_WAIT, // Ждем запуска главного меню (нажатия кнопки)
	MENU_STATE_MAIN  // Запуск главного меню
}MENU_StateTypeDef;
//-----------------------------------------------------------
typedef enum { // Структура с перечислением для главного меню
	MAIN_MENU_STATE_MM1_IDLE, // Пункт 1 главного меню
	MAIN_MENU_STATE_MM2_IDLE, // Пункт 2 главного меню
	MAIN_MENU_STATE_MM3_IDLE, // Пункт 3 главного меню
	MAIN_MENU_STATE_MM4_IDLE, // Пункт 4 главного меню
	MAIN_MENU_STATE_MM5_IDLE, // Пункт 5 главного меню
	MAIN_MENU_STATE_MM6_IDLE, // Пункт 6 главного меню
	MAIN_MENU_STATE_MM7_IDLE, // Пункт 7 главного меню
	MAIN_MENU_STATE_MM8_IDLE, // Пункт 8 главного меню
	MAIN_MENU_STATE_MM1_WAIT, // Ожидание выбора 1 пункта
	MAIN_MENU_STATE_MM2_WAIT, // Ожидание выбора 2 пункта
	MAIN_MENU_STATE_MM3_WAIT, // Ожидание выбора 3 пункта
	MAIN_MENU_STATE_MM4_WAIT, // Ожидание выбора 4 пункта
	MAIN_MENU_STATE_MM5_WAIT, // Ожидание выбора 5 пункта
	MAIN_MENU_STATE_MM6_WAIT, // Ожидание выбора 6 пункта
	MAIN_MENU_STATE_MM7_WAIT, // Ожидание выбора 7 пункта
	MAIN_MENU_STATE_MM8_WAIT, // Ожидание выбора 7 пункта
	MAIN_MENU_STATE_MM1, // Выбор пункта 1
	MAIN_MENU_STATE_MM2, // Выбор пункта 2
	MAIN_MENU_STATE_MM3  // Выбор пункта 3
}MAIN_MENU_StateTypeDef;
//-----------------------------------------------------------
typedef enum { // Структура с перечислением для подменю "Часы"
	SUBMENU_CLOCK_STATE_IDLE, // Подменю "Дата и время"
	SUBMENU_CLOCK_STATE_HOUR,   // Редактирование часа
	SUBMENU_CLOCK_STATE_MINUTE, // Редактирование минуты
	SUBMENU_CLOCK_STATE_WDAY,   // Редактирование дня недели
	SUBMENU_CLOCK_STATE_MDAY,   // Редактирование дня месяца
	SUBMENU_CLOCK_STATE_MONTH,  // Редактирование месяца
	SUBMENU_CLOCK_STATE_YEAR    // Редактирование года
}SUBMENU_CLOCK_StateTypeDef;
//-----------------------------------------------------------
typedef enum { // Структура с перечислением для подменю "Память:"
	SUBMENU_MEMORY_STATE_M1_IDLE, // Пункт 1 подменю "Память:"
	SUBMENU_MEMORY_STATE_M2_IDLE, // Пункт 2 подменю "Память:"
	SUBMENU_MEMORY_STATE_M1_WAIT, // Ожидание выбора 1 пункта
	SUBMENU_MEMORY_STATE_M2_WAIT, // Ожидание выбора 2 пункта
	SUBMENU_MEMORY_STATE_YESNO_WAIT, // Ожидание выбора "Да" или "Нет"
	SUBMENU_MEMORY_STATE_COPYPROGRESS, // Прогресс: 100% (Копирование)
	SUBMENU_MEMORY_STATE_ERASEPROGRESS, // Прогресс: 100% (Стирание)
	SUBMENU_MEMORY_STATE_WAIT // Ждем нажатия любой кнопки
}SUBMENU_MEMORY_StateTypeDef;
//-----------------------------------------------------------
typedef enum { // Структура с перечислением для подменю "Настройки"
	SUBMENU_SETTINGS_STATE_M1_IDLE, // Пункт 1 подменю "Настройки"
	SUBMENU_SETTINGS_STATE_M2_IDLE, // Пункт 2 подменю "Настройки"
	SUBMENU_SETTINGS_STATE_M3_IDLE, // Пункт 3 подменю "Настройки"
	SUBMENU_SETTINGS_STATE_M4_IDLE, // Пункт 4 подменю "Настройки"
	SUBMENU_SETTINGS_STATE_M1_WAIT, // Ожидание выбора 1 пункта
	SUBMENU_SETTINGS_STATE_M2_WAIT, // Ожидание выбора 2 пункта
	SUBMENU_SETTINGS_STATE_M3_WAIT, // Ожидание выбора 3 пункта
	SUBMENU_SETTINGS_STATE_M4_WAIT, // Ожидание выбора 4 пункта
	SUBMENU_SETTINGS_STATE_M1, // Выбор пункта 1
	SUBMENU_SETTINGS_STATE_M2, // Выбор пункта 2
	SUBMENU_SETTINGS_STATE_M3, // Выбор пункта 3
	SUBMENU_SETTINGS_STATE_M4 // Выбор пункта 4
}SUBMENU_SETTINGS_StateTypeDef;
//-----------------------------------------------------------
void MenuProcess(void); // Фызов функции меню
//-----------------------------------------------------------
#endif /* MENU_H_ */