#include "main.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

#ifndef __UART_1_H
#define __UART_1_H

// 串口1接收中断初始化
void UART1_Init(void);
// 串口1接收完成回调函数
void UART1_RxCpltCallback(UART_HandleTypeDef *huart);
// 串口1错误回调
void UART1_ErrorCallback(UART_HandleTypeDef *huart);

#endif // !__UART_1_H
