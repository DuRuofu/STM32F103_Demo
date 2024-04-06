#开发记录
### 说明：

1. 本文档由DuRuofu撰写，由DuRuofu负责解释及执行。
2. 本文主要记录使用STM32CubeMX+keil建立标准化的项目结构。

### 修订历史：

| 文档名称 | 版本 | 作者 | 时间 | 备注 |
| ---- | ---- | ---- | ---- | ---- |
| STM32CubeMX+keil项目标准化 | v1.0.0 | DuRuofu | 2023-06-12 | 首次建立 |

<div STYLE="page-break-after: always;"></div>

# STM32CubeMX+keil项目标准化

## 一、使用STM32CubeMX生成keil项目

> 这里以一个串口项目为例：
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

## 二、规范目录结构

默认情况下生成的项目结构如下：

![](attachments/Pasted%20image%2020240212144252.png)

我们需要自行添加Users文件夹用于存放我们自己编写的代码，添加Output文件夹用于存放编译产生的文件，如下：

![](attachments/Pasted%20image%2020240212144502.png)

并在Keil里设置编译输出文件路径：

![](attachments/Pasted%20image%2020240212144551.png)


## 三、进行git版本管理：

这里没什么好说的，主要是需要添加忽略文件`.gitignore`,内容建议如下：
```
*Output/*
*Output/*.hex
*.bak
*.scvd
*.ini
*.Administrator
*.hp
*.uvoptx
*DebugConfig/  
*OBJ/
```


## 四、项目代码结构及原则：

尽量不再STM32CubeMX生成的文件里做任何修改，例如下面的usart.h

![](attachments/Pasted%20image%2020240212145107.png)

我们不会在生成的usart.c里编写我们的程序，而是会新建用户usart模块，在新建的模块里引入生成的代码头文件，来实现去耦。

![](attachments/Pasted%20image%2020240212145438.png)

以上图为例，只有红框内的代码为自行编写，这样这些模块可以得到很好的复用。

![](attachments/Pasted%20image%2020240212145655.png)

main.c模块也只向下依赖app模块，由app模块负责整个项目的逻辑。

![](attachments/代码结构.svg)

# 参考链接
