/*!
    \file    main.c
    \brief   TLI_IPA demo

    \version 2022-04-18, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.


    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "exmc_sdram.h"
#include "lcd.h"
#include "joystick.h"

#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_stress.h"
#include "lv_demo_music.h"

/* MicroLIB LVGL */
__attribute__((weak, noreturn))
void __aeabi_assert(const char *expr, const char *file, int line)
{
    char str[12], *p;

    fputs("*** assertion failed: ", stderr);
    fputs(expr, stderr);
    fputs(", file ", stderr);
    fputs(file, stderr);
    fputs(", line ", stderr);

    p = str + sizeof(str);
    *--p = '\0';
    *--p = '\n';

    while (line > 0)
    {
        *--p = '0' + (line % 10);
        line /= 10;
    }

    fputs(p, stderr);

    abort();
}

__attribute__((weak))
void abort(void)
{
    for (;;);
}

void led_init(void)
{	
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOH);
	/* LED (PH7) */
	gpio_bit_set(GPIOH, GPIO_PIN_7);
	gpio_mode_set(GPIOH, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
	gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);
	
}

void key_init(void)
{
	/*
	KEY1: PH4
	KEY2: PA0
	KEY3: PG2
	*/

	/* enable the key clock */
	rcu_periph_clock_enable(RCU_GPIOH);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOG);
	rcu_periph_clock_enable(RCU_SYSCFG);

	/* configure button pin as input */
	gpio_mode_set(GPIOH, GPIO_MODE_INPUT, GPIO_PUPD_NONE,GPIO_PIN_4);
	gpio_mode_set(GPIOG, GPIO_MODE_INPUT, GPIO_PUPD_NONE,GPIO_PIN_0);
	gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE,GPIO_PIN_2);

	/* connect key EXTI line to key GPIO pin */
	syscfg_exti_line_config(EXTI_SOURCE_GPIOH, EXTI_SOURCE_PIN4);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN0);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOG, EXTI_SOURCE_PIN2);
	
	/* configure key EXTI line */
	exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	
	/* clear EXTI flag */
	exti_interrupt_flag_clear(EXTI_4);
	exti_interrupt_flag_clear(EXTI_0);
	exti_interrupt_flag_clear(EXTI_2);
	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI4_IRQn, 2U, 2U);
	nvic_irq_enable(EXTI0_IRQn, 2U, 1U);
	nvic_irq_enable(EXTI2_IRQn, 2U, 0U);
}

void usart0_init(void)
{
	/*
	TX: PA15
	RX: PB3
	*/
	
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	
	/* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
	
	/* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_15);
	
	/* connect port to USARTx_Rx */
    gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_3);
	
	/* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_15);
	
	/* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_3);
	
	/* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0,115200U);
	usart_parity_config(USART0, USART_PM_NONE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
	
	/* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 3U, 0U);
	
	/* enable USART0 receive interrupt */
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}

static void led_toggle(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_VALUE_CHANGED)
	{
		gpio_bit_toggle(GPIOH, GPIO_PIN_7);
	}
}

static void backlight_tuning(lv_event_t *e)
{
	lv_obj_t *target = lv_event_get_target(e);
	SET_LCD_BLK(lv_slider_get_value(target) * 10);
}

/*!
    \brief      main program
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure the SysTick */
    systick_config();
	
	/* configure the EXMC access mode */
	exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
	
	/* configure the LED */
	led_init();
	
	/* configure the USART */
	usart0_init();
	
	/* configure the LCD backlight */
	lcd_backlight_gpio_config();
	lcd_backlight_pwm_config();
	
	/* configure the KEY */
	key_init();
	
	/* configure the LVGL */
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	
//	lv_demo_stress();
//	lv_demo_music();
	
	lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
	lv_obj_set_size(switch_obj, 120, 60);
	lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);	
	lv_obj_add_flag(switch_obj, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_add_event_cb(switch_obj, led_toggle, LV_EVENT_VALUE_CHANGED, NULL);
	
	lv_obj_t *btn = lv_btn_create(lv_scr_act());
	lv_obj_set_size(btn, 100, 50);
	lv_obj_align(btn, LV_ALIGN_CENTER, -(lv_obj_get_width(lv_scr_act()) / 3), 0);
	lv_obj_set_style_bg_color(btn, lv_color_hex(0xde4d3e), LV_STATE_PRESSED);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_add_event_cb(btn, led_toggle, LV_EVENT_VALUE_CHANGED, NULL);
	
	lv_obj_t *backlight = lv_slider_create(lv_scr_act());
	lv_obj_set_size(backlight, 400, 20);
	lv_slider_set_value(backlight, 50, LV_ANIM_ON);
	lv_slider_set_range(backlight, 0, 100);
	lv_obj_align(backlight, LV_ALIGN_CENTER, 0,  (lv_obj_get_height(lv_scr_act()) / 3));
	lv_obj_add_event_cb(backlight, backlight_tuning, LV_EVENT_VALUE_CHANGED, NULL);
	
	while(1)
	{
		delay_1ms(1);
		lv_timer_handler();
	}
}





/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
