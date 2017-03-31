#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/*!
\brief Таблица соответствия кодов кнопок и их функционального назначения
*/
typedef enum
{
    KEY_0       = 0,
    KEY_1       = 1,
    KEY_2_UP    = 2,
    KEY_3       = 3,
    KEY_4_LEFT  = 4,
    KEY_5       = 5,
    KEY_6_RIGHT = 6,
    KEY_7       = 7,
    KEY_8_DOWN  = 8,
    KEY_9       = 9,
    KEY_EDIT      = 10,
    KEY_UP      = 13,
    KEY_RIGHT   = 14,
    KEY_LEFT    = 12,
    KEY_DOWN    = 15,
} KEY_PRESSED;


/*!
\brief Инициализация клавиатуры
*/
void Keyboard_Init(void);

/*!
\brief Процесс опроса клавиатуры
@return char 0 - кнопка не нажата, 1 кнопка нажата
*/
void KeyboardProcess(void);

/*!
\brief Установка флага нажатия кнопки
@param uchar 0 - кнопка не нажата, 1 кнопка нажата
*/
void keyboardSetBITKL(uchar press);

/*!
\brief Чтение флага нажатия на кнопки
@return char 0 - кнопка не нажата, 1 кнопка нажата
*/
uchar keyboardGetBITKL(void);

/*!
\brief Установка кода нажатой кнопки
@param KEY_PRESSED код нажатой кнопки
*/
void keyboardSetSIM(KEY_PRESSED key);

/*!
\brief Чтение кода нажатой кнопки
@return KEY_PRESSED код нажатой кнопки
*/
KEY_PRESSED keyboardGetSIM(void);

#endif
