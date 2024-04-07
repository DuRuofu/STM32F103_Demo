#include "main.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef __UART_1_H
#define __UART_1_H

// 串口1接收中断初始化
void UART1_Init(void);
// 串口1接收完成回调函数
void UARTE1_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
// 串口1错误回调
void UART1_ErrorCallback(UART_HandleTypeDef *huart);
// 串口1发送
void Usart1DmaPrintf(const char *format, ...);

#endif // !__UART_1_H
