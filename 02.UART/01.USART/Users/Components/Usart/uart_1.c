/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-07-13 17-13-53
 * @LastEditors: DuRuofu
 * @LastEditTime: 2024-04-07 09-33-37
 * @FilePath: \STM32F103_Demo\02.UART\01.USART\Users\Components\Usart\uart_1.c
 * @Description: 串口3逻辑
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved.
 */

#include "uart_1.h"

#define RXBUFFERSIZE_1 256 // 最大接收字节数

// 定义串口句柄,使用串口1
#define UART_HANDLE huart1

// 定义数据缓冲区
uint8_t RxBuffer_1[RXBUFFERSIZE_1];
uint8_t Uart_RxBuffer_1;   // 接收中断缓冲
uint8_t Uart_Rx_Cnt_1 = 0; // 接收缓冲计数

void UART1_Init(void)
{
  /*串口硬件配置代码(使用cudeMX则不需要此部分)
  Init the GPIO of USART1
  */
  // 使能 USART1 的接收中断
  __HAL_UART_ENABLE_IT(&UART_HANDLE, UART_IT_RXNE);
  // 开启 USART1 的连续接收中断，并指定接收缓冲区的地址和长度
  HAL_UART_Receive_IT(&UART_HANDLE, &Uart_RxBuffer_1, 1);
}

// 串口3接收完成回调函数
void UART1_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */

  if (Uart_Rx_Cnt_1 >= 255) // 溢出判断
  {
    Uart_Rx_Cnt_1 = 0;
    memset(RxBuffer_1, 0x00, sizeof(RxBuffer_1));
    HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)"数据溢出", 10, 0xFFFF);
  }
  else
  {
    RxBuffer_1[Uart_Rx_Cnt_1++] = Uart_RxBuffer_1;
    // 单字符判断

    if (Uart_RxBuffer_1 == '1') // 当发送1时，翻转电平
    {
      printf("发送1");
    }
    else if (Uart_RxBuffer_1 == '2') // 当发送2时，翻转电平
    {
      printf("发送2");
    }
    else if (Uart_RxBuffer_1 == '3') // 当发送3时，翻转电平
    {
      printf("发送3");
    }
    else if (Uart_RxBuffer_1 == '4') // 当发送4时，翻转电平
    {
      printf("发送4");
    }
    if ((RxBuffer_1[Uart_Rx_Cnt_1 - 1] == 0x0A) && (RxBuffer_1[Uart_Rx_Cnt_1 - 2] == 0x0D)) // 判断结束位
    {
      // 这里可以写多字节消息的判断
      HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&RxBuffer_1, Uart_Rx_Cnt_1, 0xFFFF); // 将收到的信息发送出去
      while (HAL_UART_GetState(&UART_HANDLE) == HAL_UART_STATE_BUSY_TX)
        ; // 检测UART发送结束

      // 复位
      Uart_Rx_Cnt_1 = 0;
      memset(RxBuffer_1, 0x00, sizeof(RxBuffer_1)); // 清空数组
    }
  }

  HAL_UART_Receive_IT(&UART_HANDLE, (uint8_t *)&Uart_RxBuffer_1, 1); // 因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用
}

// 串口1错误回调函数(主要用来清除溢出中断)
void UART1_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (HAL_UART_ERROR_ORE)
  {
    uint32_t temp = huart->Instance->SR;
    temp = huart->Instance->DR;
  }
}

/**
 * 函数功能: 重定向c库函数printf到DEBUG_USARTx
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
