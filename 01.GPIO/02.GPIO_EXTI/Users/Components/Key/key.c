/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-08-02 12-02-17
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-03 15-28-12
 * @FilePath: \Project\RedServo\STM32\User\KEY\key.c
 * @Description: 键盘扫描
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "key.h"
#include "led.h"
#include "app.h"


#define KEY1_Pin KEY_1_Pin
//#define KEY2_Pin KEY_2_Pin
//#define KEY3_Pin KEY_3_Pin
//#define KEY4_Pin KEY_4_Pin
//#define KEY5_Pin KEY_5_Pin




#define DEBOUNCE_DELAY 250 // 设置消抖延时为200毫秒




/**
 * @description: 按键初始化 (使用CubeMX自动生成的宏定义，就不用写这个函数了)
 * @return {*}
 */
void Key_Init(void)
{
        
}



// 按键1
void Key_1_Callback(void)
{
    Led_Toggle();
}



// 按键2
void Key_2_Callback(void)
{

}




//
/**
 * @description: 按键检测，外部中断回调函数
 * @param {uint16_t} GPIO_Pin
 * @return {*}
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

    /* Prevent unused argument(s) compilation warning */
    UNUSED(GPIO_Pin);
    /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_GPIO_EXTI_Callback could be implemented in the user file
    */

    // 按键按下
    if(GPIO_Pin == KEY1_Pin)
    {
        //printf("按键1");
        Debounce(GPIO_Pin, Key_1_Callback);
       
    }
//    else if(GPIO_Pin == KEY2_Pin)
//    {

//        // 按键2按下的处理代码
//        Debounce(GPIO_Pin, Key_2_Callback);
//    }
}




// 通用的按键消抖函数
void Debounce(uint16_t GPIO_Pin, void (*callback)(void))
{
    static uint32_t lastTriggerTime = 0;
    uint32_t currentTime = HAL_GetTick(); // 获取当前时间戳

    if (currentTime - lastTriggerTime >= DEBOUNCE_DELAY)
    {
        //Buzzer_ShortBeep();
        callback(); // 调用传入的回调函数
        lastTriggerTime = currentTime; // 更新上一次触发的时间戳
    }
}



