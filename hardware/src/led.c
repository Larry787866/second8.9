/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"

static const uint16_t led_pins[] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};
static uint8_t step  = 0;    /* 当前这组灯的第一个编号 */
static uint8_t phase = 0;    /* 0=该亮了, 1=该灭了 */
static tick_timer_t tmr;     /* 计时器 */

#define IS_VALID_LED(id) ((id) < LED_COUNT)
#define LED_COUNT 4U

/* ── 单次触发计时器 ─────────────────────────────
 * 启动后经过 period 毫秒触发一次;触发后失效,
 * 必须再次 timer_start 才会重新触发(单次触发语义) */

static void timer_start(tick_timer_t *t, uint32_t ms)
{
    t->start  = HAL_GetTick();
    t->period = ms;
    t->active = 1;
}

static uint8_t timer_expired(tick_timer_t *t)
{
    if (!t->active) {
        return 0;
    }
    if ((HAL_GetTick() - t->start) >= t->period) {
        t->active = 0;                  /* 单次:响一次就失效 */
        return 1;
    }
    return 0;
}

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

typedef enum {
    MODE_IDLE,    /* 空闲状态 */
    MODE_SEQ,   /* 逐个亮灭 */
    MODE_PAIR,  /* 两两亮灭 */
    MODE_ALL    /* 一起亮灭 */
} flow_mode_t;

static int signal = 1;
static flow_mode_t current_mode = MODE_IDLE;

static void blink(uint8_t led_num, uint8_t group, uint32_t on_ms, uint32_t off_ms)
{
    if (!timer_expired(&tmr)) {
        return;                    
    }

    if (phase == 0) {                 /* 该点亮 */
        for (uint8_t g = 0; g < group; g++) {
            led_on(led_num + g);
        }
        timer_start(&tmr, on_ms);     
        phase = 1;
    } else {                          /* 该熄灭 */
        for (uint8_t g = 0; g < group; g++) {
            led_off(led_num + g);
        }
        timer_start(&tmr, off_ms);    
        phase = 0;
        step += group;                
        if (step >= LED_COUNT) {
            step = 0;                 
        }
    }
}

void led_running(void)
{
    /* ① 信号变了 → 立刻切新模式 */
    if ((flow_mode_t)signal != current_mode) {
        current_mode = (flow_mode_t)signal;
        step  = 0;
        phase = 0;
        for (uint8_t i = 0; i < LED_COUNT; i++) {
            led_off(i);               /* 先全灭 */
        }
        if (current_mode == MODE_IDLE) {
            return;                   /* IDLE:全灭停住 */
        }
        timer_start(&tmr, 1U);        
        return;
    }

    if (current_mode == MODE_IDLE) {
        return;                       /* IDLE 不动 */
    }

    switch (current_mode)
    {
        case MODE_SEQ:
            blink(step, 1,     250U, 250U);   /* 一次 1 颗 */
            break;
        case MODE_PAIR:
            blink(step, 2,     500U, 500U);   /* 一次 2 颗 */
            break;
        case MODE_ALL:
            blink(0, LED_COUNT, 500U, 500U);  /* 一次 4 颗 */
            break;
        default:
            break;
    }
}       
