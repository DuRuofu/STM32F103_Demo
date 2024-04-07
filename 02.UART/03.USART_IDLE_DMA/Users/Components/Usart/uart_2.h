#include "main.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef __UART_2_H
#define __UART_2_H

// 串口2接收中断初始化
void UART2_Init(void);
// 串口2接收完成回调函数
void UARTE2_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
// 串口2错误回调
void UART2_ErrorCallback(UART_HandleTypeDef *huart);
// 实现printf函数
void Usart2DmaPrintf(const char *format, ...);
#endif // !__UART_2_H
