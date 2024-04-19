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
#include "led.h"
#include "spi_flash.h"
#include "audio.h"

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
	nvic_irq_enable(EXTI4_IRQn, 3U, 0U);
	nvic_irq_enable(EXTI0_IRQn, 3U, 0U);
	nvic_irq_enable(EXTI2_IRQn, 3U, 0U);
}

void uart7_init(void)
{
	/*
	TX: PE1
	RX: PE0
	*/
	
	rcu_periph_clock_enable(RCU_GPIOE);
	
	/* enable USART clock */
    rcu_periph_clock_enable(RCU_UART7);
	
	/* connect port to UARTx_Tx */
    gpio_af_set(GPIOE, GPIO_AF_8, GPIO_PIN_1);
	
	/* connect port to UARTx_Rx */
    gpio_af_set(GPIOE, GPIO_AF_8, GPIO_PIN_0);
	
	/* configure UART Tx as alternate function push-pull */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
	
	/* configure UART Rx as alternate function push-pull */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_0);
	
	/* UART configure */
    usart_deinit(UART7);
	usart_oversample_config(UART7, USART_OVSMOD_8);
    usart_baudrate_set(UART7, 6000000U);
	usart_parity_config(UART7, USART_PM_NONE);
    usart_receive_config(UART7, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART7, USART_TRANSMIT_ENABLE);
    usart_enable(UART7);
	
	/* UART interrupt configuration */
    nvic_irq_enable(UART7_IRQn, 2, 0);
	
	/* enable UART7 receive interrupt */
    usart_interrupt_enable(UART7, USART_INT_RBNE);
}

/*!
    \brief      main program
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	uint32_t id = 0;
	
    /* configure the SysTick */
    systick_config();

	/* configure the LED */
	led_init();
	
	/* configure the UART */
	uart7_init();
	
	/* configure the KEY */
	key_init();
	
	/* configure the SPI flash */
	spi_flash_init();
	
	/* configure the PWM DAC */
	pwm_dac_gpio_config();
	pwm_dac_pwm_config();
	pwm_dac_timer_config();
	mute_init();
	
	id = spi_flash_read_id();
	printf("\r\nManufacturer ID is %02x & Device ID is %02x %02x\r\n", (uint8_t)(id >> 16), (uint8_t)(id >> 8), (uint8_t)id);
	
	LED_ON();
	erase_audio_data();
	LED_OFF();
	
	recv_audio_data_from_uart();
	
	nvic_irq_disable(UART7_IRQn);
	
	LED_ON();
	delay_1ms(800);
	LED_OFF();
	
	while(1)
	{
		play_audio();
	}
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(UART7, (uint8_t)ch);
    while(RESET == usart_flag_get(UART7, USART_FLAG_TBE));
    return ch;
}
