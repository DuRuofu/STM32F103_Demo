#include "main.h"
#include "app.h"
#include "uart_it_config.h"

#define TAG "APP"

/**
 * @description: 系统应用初始化
 * @return {*}
 */
void App_Init(void)
{
	//HAL_Delay(1000);
    UART_IT_Init();
    
}


/**
 * @description: 系统应用循环任务
 * @return {*}
 */
void App_Task(void)
{
	Led_Toggle();
	HAL_Delay(1000);
	// 测试串口1发送
	Usart1DmaPrintf("[UART1] sys task runing--\r\n");
	// 测试串口2发送
	Usart2DmaPrintf("[UART2] sys task runing--\r\n");
}
