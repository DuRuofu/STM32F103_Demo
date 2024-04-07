/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-07-13 17-13-53
 * @LastEditors: DuRuofu
 * @LastEditTime: 2024-04-07 11-46-40
 * @FilePath: \STM32F103_Demo\02.UART\03.USART_IDLE_DMA\Users\Components\Usart\uart_2.c
 * @Description: 串口3逻辑
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved.
 */

#include "uart_2.h"

#define RXBUFFERSIZE_2 256 // 最大接收字节数

// 定义串口句柄,使用串口2
#define UART_HANDLE huart2

// 定义数据缓冲区
uint8_t RxBuffer_2[RXBUFFERSIZE_2];
uint8_t Uart_RxBuffer_2;   // 接收中断缓冲
uint8_t Uart_Rx_Cnt_2 = 0; // 接收缓冲计数

// 发送缓冲区
uint8_t TxBuffer_2[RXBUFFERSIZE_2];

extern DMA_HandleTypeDef hdma_usart2_rx;

void UART2_Init(void)
{
  /*串口硬件配置代码(使用cudeMX则不需要此部分)
  Init the GPIO of USART2
  */
  // 使能 USART2 的接收中断
  __HAL_UART_ENABLE_IT(&UART_HANDLE, UART_IT_RXNE);
  // 开启 USART2 的连续接收中断，并指定接收缓冲区的地址和长度
  HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, &Uart_RxBuffer_2, 1);
  // 关闭DMA的传输过半中断
  __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

// 串口2接收完成回调函数
void UARTE2_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  UNUSED(Size);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
  // 将数据发送回去
  HAL_UART_Transmit_DMA(&UART_HANDLE, RxBuffer_2, Size);
  HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, RxBuffer_2, RXBUFFERSIZE_2);
  }

// 串口2错误回调函数(主要用来清除溢出中断)
void UART2_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (HAL_UART_ERROR_ORE)
  {
    uint32_t temp = huart->Instance->SR;
    temp = huart->Instance->DR;
  }
}

// 实现printf函数
void Usart2DmaPrintf(const char *format, ...)
{
  uint16_t len;
  va_list args;
  va_start(args, format);
  len = vsnprintf((char *)TxBuffer_2, sizeof(TxBuffer_2) + 1, (char *)format, args);
  va_end(args);
  HAL_UART_Transmit_DMA(&UART_HANDLE, TxBuffer_2, len);
}
