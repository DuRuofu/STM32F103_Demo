#include "key.h"

#define KEY1_Pin KEY_1_Pin

// #define KEY2_Pin KEY_2_Pin
// #define KEY3_Pin KEY_3_Pin
// #define KEY4_Pin KEY_4_Pin
// #define KEY5_Pin KEY_5_Pin
// #define KEY6_Pin KEY_6_Pin

#define DEBOUNCE_DELAY 200 // 设置消抖延时为200毫秒


/**
 * @description: 按键初始化 (使用CubeMX自动生成的宏定义，就不用写这个函数了)
 * @return {*}
 */
void Key_Init(void){}

/**
 * @description: 按键回调函数
 * @return {*}
 */
void Key_1_Callback(void)
{
	// 按键逻辑代码
    Led_Toggle();
}


/**
 * @description: 按键检测，外部中断回调函数
 * @param {uint16_t} GPIO_Pin
 * @return {*}
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);
	/* NOTE: This function Should not be modified, when the callback is needed,
		the HAL_GPIO_EXTI_Callback could be implemented in the user file
	*/
	if (GPIO_Pin == KEY1_Pin)
	{
		Debounce(Key_1_Callback);
	}
	// else if (GPIO_Pin == KEY2_Pin)
	// {
	// 	// 按键2按下的处理代码
	// 	Debounce(GPIO_Pin, Key_2_Callback);
	// }
}


// 通用的按键消抖函数
void Debounce(void (*callback)(void))
{
	static uint32_t lastTriggerTime = 0;
	uint32_t currentTime = HAL_GetTick(); // 获取当前时间戳
	if (currentTime - lastTriggerTime >= DEBOUNCE_DELAY)
    
	{
		callback();					   // 调用传入的回调函数
		lastTriggerTime = currentTime; // 更新上一次触发的时间戳
	}
}
