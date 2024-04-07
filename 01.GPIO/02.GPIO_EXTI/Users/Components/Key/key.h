#include "main.h"
#include "led.h"

#ifndef __KEY_H
#define __KEY_H


void Key_1_Callback(void);
void Key_Init(void);
void Debounce(void (*callback)(void));
#endif // !__LED_H
