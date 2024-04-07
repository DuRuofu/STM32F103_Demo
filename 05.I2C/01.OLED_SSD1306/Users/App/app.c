#include "main.h"
#include "app.h"
#include "oled.h"

#define TAG "APP"

/**
 * @description: 系统应用初始化
 * @return {*}
 */
void App_Init(void)
{
	HAL_Delay(1000);
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0,0,"TEST:",sizeof("TEST:"));
}


/**
 * @description: 系统应用循环任务
 * @return {*}
 */
void App_Task(void)
{
	Led_Toggle();
	HAL_Delay(1000);
}
