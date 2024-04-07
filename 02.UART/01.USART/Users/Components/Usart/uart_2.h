#include "main.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

#ifndef __UART_2_H
#define __UART_2_H

// 串口2接收中断初始化
void UART2_Init(void);
// 串口2接收完成回调函数
void UART2_RxCpltCallback(UART_HandleTypeDef *huart);
// 串口2错误回调
void UART2_ErrorCallback(UART_HandleTypeDef *huart);

#endif // !__UART_2_H
