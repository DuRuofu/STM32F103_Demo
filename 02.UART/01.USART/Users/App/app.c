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
  printf("[UART1] sys task runing--\r\n");
	// 测试串口2发送
	HAL_UART_Transmit(&huart2, (uint8_t *)"[UART2] sys task runing--\r\n", sizeof("[UART2] sys task runing--\r\n"), 0xFFFF);
}
