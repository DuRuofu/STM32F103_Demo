
### 说明：

1. 本文档由DuRuofu撰写，由DuRuofu负责解释及执行。
2. 本文主要介绍STM32G0系列ADC的使用（供个人回忆）。

### 修订历史：

|  文档名称  |  版本  |  作者  |      时间      |   备注   |
| ---- | ---- | ------------ | ------ |
| STM32G0系列ADC入门 |v1.0.0| DuRuofu | 2024-02-13 | 首次建立 |
<div STYLE="page-break-after: always;"></div>
# STM32G0系列ADC入门

## 一、概念

**分辨率**：读出的数据的长度，如8位就是最大值为255的意思，即范围[0,255],12位就是最大值为4096，即范围[0,4096]

**通道**：ADC输入引脚，通常一个ADC控制器控制多个通道，如果需要多通道的话，就得进行每个通道扫描了。

**ADC DMA功能**：DMA是内存到内存或内存到存储的直接映射，数据不用经过单片机处理器而直接由硬件进行数据的传递。方便直接将读取的ADC值放到内存变量中。

ADC芯片通常有正参考电压和负参考电压，通常正参考电压连接到VCC,负参考电压连接到GND

在STM32中ADC还可以用于采集芯片的温度、RTC供电电压

**ADC****的转换方式：

1.  单次转换，一次只转换一个通道
2. 连续转换，转换完成一个通道后立即自动执行下一个通道的转换、
3. 扫描模式，开启一次后，自动的连续读取多个通道

**ADC****的三种工作方式：

1. 阻塞模式（查询模式）
2. 中断模式
3. DMA模式

在实际使用过程中我们使用最多的就是阻塞模式和DMA模式，中断模式应用不多，下面就不做演示了。
## 二、cubemx初始化项目

> 这里以STM32G070RBT6为例：

### 2.1 时钟配置

![](attachments/Pasted%20image%2020240213172643.png)

### 2.2 串口配置

![](attachments/Pasted%20image%2020240213172829.png)

### 2.3 其他配置

![](attachments/Pasted%20image%2020240213173235.png)


## 三、 单个通道，查询阻塞模式

### 3.1 ADC配置

![](attachments/Pasted%20image%2020240213175044.png)

使能ADC的0号通道，开启连续转换

业务代码：

```c
#include "adc_bsp.h"

// ADC 0号通道测量
uint32_t ADC_IN0_Meas()
{
    HAL_ADC_Start(&hadc1);//启动ADC装换
    HAL_ADC_PollForConversion(&hadc1, 50);//等待转换完成，第二个参数表示超时时间，单位ms.
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
        {
    return HAL_ADC_GetValue(&hadc1);//读取ADC转换数据，数据为12位
}
}
```

1. 使用`HAL_ADC_Start(&hadc1)`函数启动了ADC转换，其中`&hadc1`是指向ADC控制结构体的指针，用于表示使用的是哪个ADC实例。
2. 使用`HAL_ADC_PollForConversion(&hadc1, 50)`函数等待ADC转换完成，超时时间设置为50毫秒，如果在超时时间内转换完成，则继续执行后续代码。
3. 使用`HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)`条件判断语句检查ADC状态是否处于转换完成状态（EOC）。如果是，则执行下一步。
4. 使用`HAL_ADC_GetValue(&hadc1)`函数读取ADC转换结果，并将其作为返回值返回。


使用：
``` c
/**
 * @description: 系统应用循环任务
 * @return {*}
 */
void App_Task(void)
{
	uint32_t AD_Value = ADC_IN0_Meas();
	Usart3DmaPrintf("v=%.1f\r\n",AD_Value*3300.0/4096);//打印日志
}
```

效果：

![](attachments/Pasted%20image%2020240213182229.png)

![](attachments/Pasted%20image%2020240213182249.png)

## 四、DMA方式多通道采集

### 4.1 初始化多个 ADC 通道

![](attachments/Pasted%20image%2020240213183322.png)

### 4.2 ADC多通道配置

![](attachments/Pasted%20image%2020240213183522.png)

1.  使能扫描转换模式 (Scan Conversion Mode), 使能连续转换模式 (Continuous Conversion Mode)。 
2. ADC 规则组选择转换通道数为 2(Number Of Conversion)。 
3. 配置 Rank 的输入通道。

### 4.3 配置DMA

![](attachments/Pasted%20image%2020240213183717.png)

给ADC1添加DMA ，设置为连续传输模式，数据长度为字

### 4.4 编写业务代码:

添加变量。其中 ADC_Value 作为转换数据缓存数组，ad1,ad2 存 储 PA0(转换通道 0),PA1(转换通道 1) 的电压值。

```c
uint32_t ADC_Value[100]; //转换数据缓存数组
uint8_t i;               
uint32_t ad1,ad2;        // PA0(转换通道 0),PA1(转换通道 1) 的电压值
```

以 DMA 方式开启 ADC 装换
```c
void ADC_init()
{
    // ADC校准
    HAL_ADCEx_Calibration_Start(&hadc1);
    //以 DMA 方式开启 ADC 装换。HAL_ADC_Start_DMA() 函数第二个参数为数据存储起始地址，第三个参数为 DMA 传输数据的长度。
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Value, 100);
}
```

由于 DMA 采用了连续传输的模式，ADC 采集到的数据会不断传到到存储器中（此处即为数组 `ADC_Value`）。ADC 采集的数据从 `ADC_Value[0]` 一直存储到 `ADC_Value[99]`， 然后采集到的数据又重新存储到 `ADC_Value[0]`，一直到` ADC_Value[99]`。所以 `ADC_Value` 数组里面的数据会不断被刷新。这个过程中是通过 DMA 控制的，不需要 CPU 参与。我 们只需读取 `ADC_Value` 里面的数据即可得到 ADC 采集到的数据。其中 `ADC_Value[0]` 为 通道 0(PA0) 采集的数据，`ADC_Value[1]` 为通道 1(PA1) 采集的数据，`ADC_Value[2]` 为通 道 0 采集的数据，如此类推。数组偶数下标的数据为通道 0 采集数据，数组奇数下标的 数据为通道 1 采集数据。

添加应用程序，将采集的数据装换为电压值并输出。

``` c 
// ADC测量
void ADC_Meas(void)
{
    ad1 = (float)ADC_Value[0] * (3.3/4096);
    ad2 = (float)ADC_Value[1] * (3.3/4096);

    Usart3DmaPrintf("AD1_value=%1.3f,AD2_value=%1.3f\r\n", ad1,ad2);
    HAL_Delay(10);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Value, 2);
}
```

这里值得注意的是我在测量完再次调用了`HAL_ADC_Start_DMA`，这是由于一个目前尚未解决的bug：开启DMA连续转化后，但是DMA还是只搬运一次就结束了，只好再次开启。目前尚未找到解决办法。但这样也能实现功能。

### 4.5 效果：

![](attachments/Pasted%20image%2020240214125527.png)

![](attachments/Pasted%20image%2020240214125544.png)



# 参考链接

1. https://blog.csdn.net/u012121390/article/details/113363173
2. https://hui-shao.cn/stm32-adc-dma-continue/
3. https://www.bilibili.com/video/BV1q4411d7RX/?spm_id_from=333.337.search-card.all.click