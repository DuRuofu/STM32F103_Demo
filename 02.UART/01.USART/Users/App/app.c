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
    printf("sys task runing--\r\n");
}
