/**
 * @file    led.h
 * @brief   LED 驱动头文件。
 *          头文件放声明和宏，具体实现放在 led.c。
 */
#ifndef LED_H
#define LED_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 宏定义：给 LED 使用的端口和引脚起名字 */
#define LED_GPIO_PORT GPIOA
#define LED1_PIN      GPIO_PIN_4
#define LED2_PIN      GPIO_PIN_5
#define LED3_PIN      GPIO_PIN_6
#define LED4_PIN      GPIO_PIN_7

/* 种子工程先提供无参数版本，只操作 LED1；题目 1 将其扩展为带编号参数版本 */
void led_on(void);
void led_off(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
