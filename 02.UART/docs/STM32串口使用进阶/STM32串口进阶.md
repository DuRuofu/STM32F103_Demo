
### 说明：

1. 本文档由DuRuofu撰写，由DuRuofu负责解释及执行。

### 修订历史：

| 文档名称 | 版本 | 作者 | 时间 | 备注 |
| ---- | ---- | ---- | ---- | ---- |
| STM32串口进阶 | v1.0.0 | DuRuofu | 2024-02-12 | 已同步至博客 |
<div STYLE="page-break-after: always;"></div>
# STM32串口进阶

## 一、前言

我们使用基本的串口接收中断配合回调函数，已经可以完成很多功能。详见[STM32串口使用（HAL库）](../STM32串口使用（HAL库）/STM32串口使用（HAL库）.md)一文。

``` c

#include "uart_it_config.h"

/**
 * @description: 串口接收中断初始化(总)
 * @return {*}Debug_Init
 */
void UART_IT_Init(void)
{
    //串口1接收中断初始化
    //USART1_Init();
    //串口2接收中断初始化
    //USART2_Init();
    //串口3接收中断初始化
    UART3_Init();
}

// 串口接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        //UART1_RxCpltCallback(huart);
    }
    else if(huart->Instance == USART2)
    {
        //UART2_RxCpltCallback();
    }
    else if(huart->Instance == USART3)
    {
        UART3_RxCpltCallback(huart);
    }
}
```

我们使用`uart_it_config`模块代理全局的串口接收数据非空中断`HAL_UART_RxCpltCallback`，然后在内部实现各个串口的逻辑，以`UART3`为例：

``` c
#include "uart_3.h"

#define RXBUFFERSIZE_3 256 // 最大接收字节数

// 定义串口句柄,使用串口1
#define UART_HANDLE huart3


uint8_t RxBuffer_3[RXBUFFERSIZE_3];  // 定义数据缓冲区
uint8_t Uart_RxBuffer_3;             // 接收中断缓冲
uint8_t Uart_Rx_Cnt_3 = 0;           // 接收缓冲计数

void UART3_Init(void)
{
  /*串口硬件配置代码(使用cudeMX则不需要此部分)
  Init the GPIO of USART1
  */
  // 使能 USART1 的接收中断
  __HAL_UART_ENABLE_IT(&UART_HANDLE, UART_IT_RXNE);
  // 开启 USART1 的连续接收中断，并指定接收缓冲区的地址和长度
  HAL_UART_Receive_IT(&UART_HANDLE, &Uart_RxBuffer_3, 1);
}

// 串口3接收完成回调函数
void UART3_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */

  if (Uart_Rx_Cnt_3 >= 255) // 溢出判断
  {
    Uart_Rx_Cnt_3 = 0;
    memset(RxBuffer_3, 0x00, sizeof(RxBuffer_3));
    HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)"数据溢出", 10, 0xFFFF);
  }
  else
  {
    RxBuffer_3[Uart_Rx_Cnt_3++] = Uart_RxBuffer_3;
    // 单字符判断

    if ((RxBuffer_3[Uart_Rx_Cnt_3 - 1] == 0x0A) && (RxBuffer_3[Uart_Rx_Cnt_3 - 2] == 0x0D)) // 判断结束位
    {
      // 这里可以写多字节消息的判断
      HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&RxBuffer_3, Uart_Rx_Cnt_3, 0xFFFF); // 将收到的信息发送出去
      while (HAL_UART_GetState(&UART_HANDLE) == HAL_UART_STATE_BUSY_TX); // 检测UART发送结束

      // 复位
      Uart_Rx_Cnt_3 = 0;
      memset(RxBuffer_3, 0x00, sizeof(RxBuffer_3)); // 清空数组
    }
  }

  // 重新使能串口接收中断
  HAL_UART_Receive_IT(&UART_HANDLE, (uint8_t *)&Uart_RxBuffer_3, 1); // 因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用
}

// 串口3错误回调函数(主要用来清除溢出中断)
//  void UART3_ErrorCallback(UART_HandleTypeDef *huart)
//  {
//    if(HAL_UART_ERROR_ORE)
//    {
//       uint32_t temp = huart->Instance->SR;
//       temp = huart->Instance->DR;
//    }
//  }

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
```

`UART3_RxCpltCallback`函数通过判断接收计数器`Uart_Rx_Cnt_3`的值是否达到上限255，来检测是否发生了溢出。如果没有溢出，将接收到的数据存储到接收缓冲区`RxBuffer_3`中，并进行结束位的判断。这里只给出了单字符结束位的判断，即判断收到的字符是否是0x0D和0x0A（对应回车换行）。如果满足结束位的条件，则将接收到的信息通过串口发送出去，并等待发送结束。最后，重新使能串口接收中断，以便继续接收后续的数据。

这样就可以实现结尾为`\r\n`，的不定长数据接收，但 并不是真正的接收不定长数据，只能算是"伪不定长"(但实际上也能满足业务需求)。




## 二、串口单字节连续接收数据+DMA

上面这样一个字节一个字节的处理，频繁中断会占用cpu时间，我们可以引入DMA代替cpu搬运数据。


为串口添加DMA通道：

![](attachments/Pasted%20image%2020240213105705.png)


为UART_TX,UART_RX分别创建DMA通道。

![](attachments/Pasted%20image%2020240213105900.png)


其余的变化不大，只需要再前言的代码基础上将`HAL_UART_Receive_IT`改为`HAL_UART_Receive_DMA`
将`HAL_UART_Transmit`改为`HAL_UART_Transmit_DMA`,

**串口DMA方式接收函数：`HAL_UART_Receive_DMA`**

|函数原型|HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_Handle TypeDef *huart, uint 8_t *pData, uint 16_t Size)|
|---|---|
|功能描述|在DMA方式下接收一定数量的数据|
|入口参数1|huart：串口句柄的地址|
|入口参数|pData：待接收数据的首地址|
|入口参数3|Size：待接收数据的个数|
|返回值|HAL状态值：HAL_OK表示发送成功；HAL_ERROR表示参数错误；HAL_BUSY表示串口被占用；|
|注意事项|1. 该函数将启动DMA方式的串口数据接收2. 完成指定数量的数据接收后，可以触发DMA中断，在中断中将调用接收中断回调函数HAL_UART_ExCpltCallback进行后续处理3. 该函数由用户调用户调用|



**串口DMA方式发送函数：`HAL_UART_Transmit_DMA`**

|函数原型|HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_Handle TypeDef *huart, uint 8_t *pData, uint 16_t Size)|
|---|---|
|功能描述|在DMA方式下发送一定数量的数据|
|入口参数1|huart：串口句柄的地址|
|入口参数|pData：待发送数据的首地址|
|入口参数3|Size：待发送数据的个数|
|返回值|HAL状态值：HAL_OK表示发送成功；HAL_ERROR表示参数错误；HAL_BUSY表示串口被占用；|
|注意事项|1. 该函数将启动DMA方式的串口数据发送2. 完成指定数量的数据发送后，可以触发DMA中断，在中断中将调用发送中断回调函数HAL_UART_TxCpltCallback进行后续处理3. 该函数由用户调用户调用|


这里有一点值得注意：

```c

// 这里可以写多字节消息的判断
HAL_UART_Transmit_DMA(&UART_HANDLE, (uint8_t *)&RxBuffer_3, Uart_Rx_Cnt_3, 0xFFFF); // 将收到的信息发送出去
while (HAL_UART_GetState(&UART_HANDLE) == HAL_UART_STATE_BUSY_TX); // 检测UART发送结束
```

修改为DMA后，这里判断发送结束的函数将失去作用，因此要去掉这句。

还有就是STM32以DMA方式实现printf函数和普通中断方式不一样。
实现printf函数主要就是要改写fputc函数
``` c
int fputc(int ch, FILE *f)
```

但是fputc函数每次只能发送一个字节，如果我们把fputc函数直接改成：
``` c
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit_DMA(&PrintUartHandle, (uint8_t *)&ch, 1); 
  return (ch);
}
```

那么至少存在两个问题：
1、DMA每次都发送一个字节，效率比较低。
2、频繁调用fputc，可能DMA上一次的数据还没有发送完，导致这次发送失败。
例如：
```c
printf("HelloWorld\r\n");
```

printf先发送H,调用fputc函数，此时DMA开始工作。由之前的分析可知，对于9600bps来说，需要1ms才能把字符H发送完成。在这1ms之内elloWolrd\r\n都会调用fputc函数，但由于DMA还没有发送完成，会导致其他的字符发送失败。最终成功发出去的只有第一个字符H。

如果想用调用DMA，就要想其他的办法。

简单起见，我们自定义一个使用DMA的打印函数：
``` c
// 发送缓冲区
#define UART_TX_BUF_SIZE 256
uint8_t UartTxBuf[UART_TX_BUF_SIZE];

void Usart3DmaPrintf(const char *format,...)
{
	uint16_t len;
	va_list args;	
	va_start(args,format);
	len = vsnprintf((char*)UartTxBuf,sizeof(UartTxBuf)+1,(char*)format,args);
	va_end(args);
	HAL_UART_Transmit_DMA(&huart3, UartTxBuf, len);
}
```

- 在函数开始时，使用 va_start 宏初始化了一个 va_list args 变量，以便访问可变数量的参数。
- 然后，使用 vsnprintf 函数将格式化后的字符串写入到 UartTxBuf 缓冲区中，并返回生成的字符串长度 len。vsnprintf 函数的第一个参数是目标缓冲区的地址，第二个参数是缓冲区大小，第三个参数是格式化字符串，最后一个参数是一个 va_list 变量，用于获取可变数量的参数列表。
- 在调用 vsnprintf 函数之后，使用 va_end 宏清理 va_list 变量，并结束对可变数量参数的访问。
- 最后，使用 HAL 库函数 HAL_UART_Transmit_DMA 发送数据。该函数的参数包括串口句柄 huart3、待发送数据的缓冲区 UartTxBuf，以及数据长度 len（注意：在代码中，len 实际上比 UartTxBuf 的大小小 1，因为 vsnprintf 已经将字符串末尾的 '\0' 字符计算在内了）。

修改后的代码如下：

``` c
#include "uart_3.h"

#define RXBUFFERSIZE_3 256 // 最大接收字节数

// 定义串口句柄,使用串口1
#define UART_HANDLE huart3

// 定义数据缓冲区
uint8_t RxBuffer_3[RXBUFFERSIZE_3];
uint8_t Uart_RxBuffer_3;   // 接收中断缓冲
uint8_t Uart_Rx_Cnt_3 = 0; // 接收缓冲计数

// 发送缓冲区
#define UART_TX_BUF_SIZE 256
uint8_t UartTxBuf[UART_TX_BUF_SIZE];


void UART3_Init(void)
{
  /*串口硬件配置代码(使用cudeMX则不需要此部分)
  Init the GPIO of USART1
  */
  // 使能 USART1 的接收中断
  __HAL_UART_ENABLE_IT(&UART_HANDLE, UART_IT_RXNE);
  // 开启 USART1 的连续接收中断，并指定接收缓冲区的地址和长度
  HAL_UART_Receive_DMA(&UART_HANDLE, &Uart_RxBuffer_3, 1);
}

// 串口3接收完成回调函数
void UART3_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */

  if (Uart_Rx_Cnt_3 >= 255) // 溢出判断
  {
    Uart_Rx_Cnt_3 = 0;
    memset(RxBuffer_3, 0x00, sizeof(RxBuffer_3));
    Usart3DmaPrintf("数据溢出");
  }
  else
  {
    RxBuffer_3[Uart_Rx_Cnt_3++] = Uart_RxBuffer_3;
    // 单字符判断

    if ((RxBuffer_3[Uart_Rx_Cnt_3 - 1] == 0x0A) && (RxBuffer_3[Uart_Rx_Cnt_3 - 2] == 0x0D)) // 判断结束位
    {
        char str[256];
        sprintf(str, "%s", RxBuffer_3);
        Usart3DmaPrintf(str); //将收到的信息发送出去

        // 复位
        Uart_Rx_Cnt_3 = 0;
        memset(RxBuffer_3, 0x00, sizeof(RxBuffer_3)); // 清空数组
    }
  }

  HAL_UART_Receive_DMA(&UART_HANDLE, (uint8_t *)&Uart_RxBuffer_3, 1); // 因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用
}

// 串口3错误回调函数(主要用来清除溢出中断)
//  void UART3_ErrorCallback(UART_HandleTypeDef *huart)
//  {
//    if(HAL_UART_ERROR_ORE)
//    {
//       uint32_t temp = huart->Instance->SR;
//       temp = huart->Instance->DR;
//    }
//  }


/**
 * @description: 串口3发送函数
 * @param {char} *format
 * @return {*}
 */
void Usart3DmaPrintf(const char *format,...)
{
	uint16_t len;
	va_list args;	
	va_start(args,format);
	len = vsnprintf((char*)UartTxBuf,sizeof(UartTxBuf),(char*)format,args);
	va_end(args);
	HAL_UART_Transmit_DMA(&huart3, UartTxBuf, len);
}
```

值得注意的是，发送部分使用了自己封装的` Usart3DmaPrintf(str);`函数。

效果如下：

![](attachments/Pasted%20image%2020240213114553.png)

使用DMA进一步减轻了CPU的负担，但是这样依旧只能一个字节一个字节接收。不是真正的不定长数据接收。
## 三、串口空闲中断接收不定长数据

为了完成接收真正的不定长数据，我们可以使用串口空闲中断，在一帧数据接收完再进行处理，而不是上面这样一个字节一个字节的处理。

空闲中断是串口RX总线上在一个字节的时间内没有再接收到数据的时候发生的。

对于HAL库，使用空闲中断接收，只需要在上面的代码基础上将`HAL_UART_Receive_DMA`改为`HAL_UARTEx_ReceiveToIdle_DMA` 当然，对于空闲中断也可以不使用DMA，这里我们直接使用DMA。

如何将串口接收寄存器非空回调更改为：`HAL_UARTEx_RxEventCallback`串口空闲中断回调函数

对几个串口的空闲中断进行代理：

``` c
// 串口空闲中断回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance == USART1)
    {
        //UARTx1_RxEventCallback(huart);
    }
    else if(huart->Instance == USART2)
    {
        //UARTx2_RxEventCallback();
    }
    else if(huart->Instance == USART3)
    {
        UARTx3_RxEventCallback(huart,Size);
    }
}
```

在串口模块自行实现逻辑

``` c
// 串口3空闲中断回调函数
void UARTx3_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  UNUSED(Size);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
  
  // 将数据发送回去
  HAL_UART_Transmit_DMA(&UART_HANDLE, RxBuffer_3,Size );
  HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, RxBuffer_3, RXBUFFERSIZE_3);
    
}
```

这样就真正实现了任意长度数据接收，并且没有任何格式要求了。

![](attachments/Pasted%20image%2020240213121756.png)

值得注意的是，DMA的传输过半中断也会触发HAL_UARTEx_RxEventCallback回调函数，在目前的例子里是不需要的，反而会造成问题。所以我们要手动关闭DMA的传输过半中断。

在开启接收中断的同时关闭DMA的传输过半中断。

```c
  HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, RxBuffer_3, RXBUFFERSIZE_3);
  __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
```
如果报错需要在模块前面添加：`extern DMA_HandleTypeDef hdma_usart3_rx;`

这样就完成了串口模块的进阶。


# 参考链接

1. https://blog.csdn.net/soledade/article/details/129030082
2. https://zhuanlan.zhihu.com/p/622278829
3. https://blog.csdn.net/youmeichifan/article/details/103133537
4. https://www.bilibili.com/video/BV1do4y1F7wt/?spm_id_from=333.1007.top_right_bar_window_history.content.click&vd_source=ef5a0ab0106372751602034cdd9ab98e