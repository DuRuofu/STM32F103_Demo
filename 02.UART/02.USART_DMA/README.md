## 串口单字节连续接收数据+DMA

内容为两个串口的不定长数据收发（DMA模式）（伪不定长，要求接收的数据后两位为`\r\n`）

注意：

串口线不能悬空，悬空容易误触串口接收中断，导致程序卡死。