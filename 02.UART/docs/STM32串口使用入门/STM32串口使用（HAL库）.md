
#开发记录
### 说明：

1. 本文档由DuRuofu撰写，由DuRuofu负责解释及执行。

### 修订历史：

| 文档名称 | 版本 | 作者 | 时间 | 备注 |
| ---- | ---- | ---- | ---- | ---- |
| STM32串口使用（HAL库） | v1.0.0 | DuRuofu | 2023-07-09 | 首次建立 |
|  |  |  |  |  |

# STM32串口使用（HAL库）

## 一、工程创建

#### 1.设置RCC

设置高速外部时钟HSE 选择外部时钟源

![](attachments/Pasted%20image%2020230709112156.png)

#### 2.设置串口
![](attachments/Pasted%20image%2020230709112203.png)
1. 点击USATR1   
2. 设置MODE为异步通信(Asynchronous)       
3. 基础参数：波特率为115200 Bits/s。传输数据长度为8 Bit。奇偶检验无，停止位1      接收和发送都使能 
4. GPIO引脚设置 USART1_RX/USART_TX
5. NVIC Settings 一栏使能接收中断
![](attachments/Pasted%20image%2020230709112215.png)

#### 3.设置时钟
![](attachments/Pasted%20image%2020230709112227.png)

我的是  外部晶振为8MHz 
1选择外部时钟HSE 8MHz   
2PLL锁相环倍频72倍
3系统时钟来源选择为PLL
4设置APB1分频器为 /2

#### 4.项目文件设置
![](attachments/Pasted%20image%2020230709112235.png)

1.  设置项目名称
2.  设置存储路径
3.  选择所用IDE


#### 5.创建工程文件 
然后点击**GENERATE CODE**  创建工程
配置下载工具
新建的工程所有配置都是默认的  我们需要自行选择下载模式，勾选上下载后复位运行
![](attachments/Pasted%20image%2020230709112243.png)

## 二、串口发送

### 1. 相关函数：
```
HAL_UART_Transmit();串口发送数据，使用超时管理机制 
HAL_UART_Receive();串口接收数据，使用超时管理机制
HAL_UART_Transmit_IT();串口中断模式发送  
HAL_UART_Receive_IT();串口中断模式接收
HAL_UART_Transmit_DMA();串口DMA模式发送
HAL_UART_Transmit_DMA();串口DMA模式接收
```
串口发送数据函数：
```c
HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
//功能：串口发送指定长度的数据。如果超时没发送完成，则不再发送，返回超时标志（HAL_TIMEOUT）。

```

### 2.以重新定义printf函数的方式发送
在生成的的usart.c里 中包含#include <stdio.h>  **重写fget和fput函数**
``` c
/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
 
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}
```
也可以自己建立文件，引入自动生成的usart.c作为依赖，在自己的模块里添加，强烈建议这种方式。

## 3.使用
```c
 
  while (1)
  {
    /* USER CODE END WHILE */
			printf("duruofu测试\n");
			HAL_Delay(1000);
    /* USER CODE BEGIN 3 */
  }
```
![](attachments/Pasted%20image%2020230709121537.png)

## 二、串口接收

这里使用中断接收：
只需要在接收中断回调函数里编写自己的代码即可：（下面这段代码也建议放在自己的模块里：）
```c
//串口接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
 
		HAL_UART_Transmit(&huart1,&Uart1RX_Data,1,0xFFFF);
        HAL_UART_Receive_IT(&huart1,&Uart1RX_Data,1);
        
	}
	
	if(huart->Instance == USART2)
	{
 
		HAL_UART_Transmit(&huart2,&Uart2RX_Data,1,0xFFFF);
        HAL_UART_Receive_IT(&huart2,&Uart2RX_Data,1);
	}
}
```

 使用 `if` 语句判断是哪个串口接收到了数据。如果是 USART1，会执行相应的操作：
- 调用 `HAL_UART_Transmit` 函数将接收到的数据 `Uart1RX_Data` 发送出去，第三个参数表示要发送的数据长度为 1。
- 调用 `HAL_UART_Receive_IT` 函数开启 USART1 的连续接收中断，继续准备接收下一组数据。
这里需要注意：
若定长串口中断接收数据，数据溢出，将会产生数据溢出错误，中断不再接收数据
这和前面的开启接收中断有关：
``` c
  //使能 USART1 的接收中断
  __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
  //开启 USART1 的连续接收中断，并指定接收缓冲区的地址和长度
  HAL_UART_Receive_IT(&huart1,&Uart1RX_Data,8);
```

超出缓冲区就会发生溢出，然后不能再接收

可以参考下面修改后的代码：

最后代码总结为：
serial_it_config.c

这段代码实现了串口接收中断相关的初始化和回调函数。
```c
#include "serial_it_config.h"

  

/**

 * @description: 串口接收中断初始化(总)

 * @return {*}Debug_Init

 */

void USART_IT_Config(void)

{

    //串口1接收中断初始化

    Debug_Init();

    //串口2接收中断初始化

  

    //串口3接收中断初始化

}

  

//串口接收完成回调函数

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)

{

    if(huart->Instance == USART1)

    {

        UART1_RxCpltCallback(huart);

    }

    else if(huart->Instance == USART2)

    {

        //UART2_RxCpltCallback();

    }

    else if(huart->Instance == USART3)

    {

        //UART2_RxCpltCallback();

    }

    }

  
  

//错误回调  

 void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)  

{      

    if(huart->Instance == USART1)

    {

        UART1_ErrorCallback(huart);

    }

    else if(huart->Instance == USART2)

    {

        //UART2_ErrorCallback();

    }

    else if(huart->Instance == USART3)

    {

        //UART3_ErrorCallback();

    }

}
```

serial_it_config.h
```c
#include "main.h"

#include "debug.h"
#ifndef __USART_CONFIG_H

#define __USART_CONFIG_H

void USART_IT_Config(void);
#endif // !__USART_CONFIG_H
```

debug.c
这段代码是用于在嵌入式系统中实现串口调试功能的代码。主要包含了串口初始化函数、接收完成回调函数、错误回调函数、发送字符串函数以及重定向c库函数printf和scanf到串口的功能。
代码中使用了名为`UART_HANDLE`的宏来定义了串口句柄，指定了使用的是串口1。同时定义了接收缓冲区和接收缓冲计数变量。在串口初始化函数`Debug_Init`中，使能了串口的接收中断，并设置连续接收中断并指定接收缓冲区的地址和长度。
接收完成回调函数`UART1_RxCpltCallback`中，判断了接收缓冲区溢出的情况，并进行了处理。然后根据接收到的字符进行操作，例如当接收到字符'1'时，输出"发送1"；当接收到字符'2'时，输出"发送2"；以此类推。最后判断是否接收到结束标志位，并把接收到的信息发送出去。
错误回调函数`UART1_ErrorCallback`用于清除溢出中断。发送字符串函数`Usart_SendString`用于发送字符串。重定向c库函数printf和scanf到串口的功能分别在`fputc`和`fgetc`函数中实现。
```c
#include "debug.h"

#include "string.h"

  

#define RXBUFFERSIZE  256     //最大接收字节数

  

//定义串口句柄,使用串口1

#define UART_HANDLE huart1

  

//定义数据缓冲区

uint8_t RxBuffer[RXBUFFERSIZE];

uint8_t Uart_RxBuffer;      //接收中断缓冲

uint8_t Uart_Rx_Cnt = 0;     //接收缓冲计数

  

void Debug_Init(void)

{

  /*串口硬件配置代码(使用cudeMX则不需要此部分)

  Init the GPIO of USART1

  */

  //使能 USART1 的接收中断

  __HAL_UART_ENABLE_IT(&UART_HANDLE,UART_IT_RXNE);

  //开启 USART1 的连续接收中断，并指定接收缓冲区的地址和长度

  HAL_UART_Receive_IT(&UART_HANDLE,&Uart_RxBuffer,1);

}

  

//串口1接收完成回调函数

void UART1_RxCpltCallback(UART_HandleTypeDef *huart)

{

    /* Prevent unused argument(s) compilation warning */

  UNUSED(huart);

  /* NOTE: This function Should not be modified, when the callback is needed,

           the HAL_UART_TxCpltCallback could be implemented in the user file

   */

  if(Uart_Rx_Cnt >= 255)  //溢出判断

  {

    Uart_Rx_Cnt = 0;

    memset(RxBuffer,0x00,sizeof(RxBuffer));

    HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)"数据溢出", 10,0xFFFF);  

  }

  else

  {

    RxBuffer[Uart_Rx_Cnt++] = Uart_RxBuffer;  

    //单字符判断

    if(Uart_RxBuffer == '1')//当发送1时，翻转电平

    {

      DEBUG_printf("发送1");

    }

    else if(Uart_RxBuffer == '2')//当发送2时，翻转电平

    {

      DEBUG_printf("发送2");

    }

    else if(Uart_RxBuffer == '3')//当发送3时，翻转电平

    {

      DEBUG_printf("发送3");

    }

    else if(Uart_RxBuffer == '4')//当发送4时，翻转电平

    {

      DEBUG_printf("发送4");

    }

    if((RxBuffer[Uart_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart_Rx_Cnt-2] == 0x0D)) //判断结束位

    {

      //这里可以写多字节消息的判断

      HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&RxBuffer, Uart_Rx_Cnt,0xFFFF); //将收到的信息发送出去

      //while(HAL_UART_GetState(&UART_HANDLE) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束

  

      //复位

      Uart_Rx_Cnt = 0;

      memset(RxBuffer,0x00,sizeof(RxBuffer)); //清空数组

    }

  }

  HAL_UART_Receive_IT(&UART_HANDLE, (uint8_t *)&Uart_RxBuffer, 1);   //因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用

  

}

  

//串口1错误回调函数(主要用来清除溢出中断)

void UART1_ErrorCallback(UART_HandleTypeDef *huart)

{

  if(HAL_UART_ERROR_ORE)

  {

     uint32_t temp = huart->Instance->SR;

     temp = huart->Instance->DR;

  }

}

  

/*****************  发送字符串 **********************/

void Usart_SendString(uint8_t *str)

{

  unsigned int k=0;

  do

  {

      HAL_UART_Transmit(&UART_HANDLE,(uint8_t *)(str + k) ,1,1000);

      k++;

  } while(*(str + k)!='\0');

}

  

/**

  * 函数功能: 重定向c库函数printf到DEBUG_USARTx

  * 输入参数: 无

  * 返 回 值: 无

  * 说    明：无

  */

int fputc(int ch, FILE *f)

{

  HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&ch, 1, 0xffff);

  return ch;

}

/**

  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx

  * 输入参数: 无

  * 返 回 值: 无

  * 说    明：无

  */

int fgetc(FILE *f)

{

  uint8_t ch = 0;

  HAL_UART_Receive(&UART_HANDLE, &ch, 1, 0xffff);

  return ch;

}
```



debug.h
```c
#include <stdio.h>

#include "main.h"

#include "usart.h"

#ifndef __DEBUG_H

#define __DEBUG_H

#define DEBUG

#ifdef DEBUG

    #define DEBUG_printf(format, ...) printf(format "\r\n", ##__VA_ARGS__)

    #define DEBUG_info(tag,format, ...) printf("DEBUG_info["tag"]:" format "\r\n", ##__VA_ARGS__)

    #define DEBUG_warnig(tag,format, ...) printf("DEBUG_warnig["tag"]:" format "\r\n", ##__VA_ARGS__)

    #define DEBUG__error(tag,format, ...) printf("DEBUG__error["tag"]:" format "\r\n",##__VA_ARGS__)

#else
    #define DEBUG_printf(format, ...) printf(format "\r\n", ##__VA_ARGS__)

    #define DEBUG_info(tag,format, ...)

    #define DEBUG_warnig(tag,format, ...)

    #define DEBUG__error(tag,format, ...)

#endif

//串口1接收中断初始化

void Debug_Init(void);

//串口1接收完成回调函数

void UART1_RxCpltCallback(UART_HandleTypeDef *huart);

void UART1_ErrorCallback(UART_HandleTypeDef *huart);

#endif // !__DEBUG_H
```



# 参考链接

####  STM32CubeMX之串口使用（中断方式）
https://zhuanlan.zhihu.com/p/162732368

#### 【STM32】HAL库——串口中断通信(二)
https://blog.csdn.net/weibo1230123/article/details/80596220








