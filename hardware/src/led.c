/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"

static const uint16_t led_pins[] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};

#define IS_VALID_LED(id) ((id) < LED_COUNT)
#define LED_COUNT 4U
/* 点亮 LED1 */
static void led_on(uint8_t led_num)
{
    if (!IS_VALID_LED(led_num)) {
        return; 
    }

    HAL_GPIO_WritePin(LED_GPIO_PORT, led_pins[led_num], GPIO_PIN_SET);
}

/* 熄灭 LED1 */
static void led_off(uint8_t led_num)
{    
    if (!IS_VALID_LED(led_num)) {
        return; 
    }

    HAL_GPIO_WritePin(LED_GPIO_PORT, led_pins[led_num], GPIO_PIN_RESET);
}


static void blink(led_config_t config)
{
    if (!IS_VALID_LED(config.led_num)) {
        return; 
    }

    led_on(config.led_num);
    HAL_Delay(config.led_on_time);
    led_off(config.led_num);
    HAL_Delay(config.led_off_time);
}       

void led_running(void)
{
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        led_config_t config = {.led_num = i, .led_on_time = 250U, .led_off_time = 250U};
        blink(config);
    }
}
