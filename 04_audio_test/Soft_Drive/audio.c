/*!
    \file    audio.c
    \brief   audio driver

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
#include <string.h>
#include <stdio.h>
#include "systick.h"

#include "audio.h"
#include "audio_table.h"
#include "spi_flash.h"
#include "led.h"

uint8_t uart_recv_buf[UART_RECV_LEN] = {0x00};
uint16_t uart_recv_cnt = 0;
uint8_t uart_recv_flag = 0;
uint32_t flash_write_cnt = 0;

uint8_t play_flag = 0;
uint8_t current_audio = 3;

uint8_t pwm_buf_1[PWM_BUF_LEN] = {0x00};
uint8_t pwm_buf_2[PWM_BUF_LEN] = {0x00};
uint8_t *pwm_buf = pwm_buf_1;
uint16_t pwm_cnt = 0;
uint8_t pwm_refresh_flag = 0;
uint32_t pwm_data_addr = 0;

void pwm_dac_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
	/* Configure PA5(TIMER1_CH0) as alternate function */
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_5);
}

void pwm_dac_pwm_config(void)
{
	/* TIMER1 configuration: generate PWM signals with different duty cycles */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 1 - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 256 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH0 configuration in PWM mode 0 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1,TIMER_CH_0,&timer_ocintpara);

    /* CH1 configuration in PWM mode 0 */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, 0);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* TIMER1 enable */
    timer_enable(TIMER1);
}

void pwm_dac_timer_config(void)
{
	/* TIMER5 configuration: generate timer interrupt */
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER5);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);

    timer_deinit(TIMER5);

    /* TIMER5 configuration */
    timer_initpara.prescaler         = 15 - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1000 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER5,&timer_initpara);
	
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER5);
	
	/* update interrupt enable */
	timer_interrupt_enable(TIMER5, TIMER_INT_UP);
	
    /* TIMER5 enable */
    timer_enable(TIMER5);
}

void mute_init(void)
{	
    /* enable the mute clock */
    rcu_periph_clock_enable(RCU_GPIOG);
	
	/* MUTE (PG3) */
	gpio_bit_set(GPIOG, GPIO_PIN_3);
	gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
	gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_3);	
}


void erase_audio_data(void)
{
	uint32_t sector = 0;
	while(sector < audio_end_addr_list[AUDIO_NUM - 1])
	{
		spi_flash_sector_erase(sector);
		sector += 0x1000;
	}
}

void recv_audio_data_from_uart(void)
{
	while(1)
	{
		if(gpio_input_bit_get(GPIOG, GPIO_PIN_2) == 0)
			break;
		if(uart_recv_flag)
		{
			uart_recv_flag = 0;
			
			spi_flash_page_write(uart_recv_buf, flash_write_cnt * UART_RECV_LEN, UART_RECV_LEN);
			
			printf("Write  %-5d OK", flash_write_cnt);
			flash_write_cnt++;
			
		}
	}
}

void play_audio(void)
{
	if(play_flag)
	{
		if(pwm_refresh_flag)
		{
			pwm_refresh_flag = 0;
			if(pwm_buf == pwm_buf_1)
			{
				pwm_buf = pwm_buf_2;
				spi_flash_buffer_read(pwm_buf_1, pwm_data_addr, PWM_BUF_LEN);
			}
			else
			{
				pwm_buf = pwm_buf_1;
				spi_flash_buffer_read(pwm_buf_2, pwm_data_addr, PWM_BUF_LEN);
			}
			
			pwm_data_addr += PWM_BUF_LEN;
			
			if(pwm_data_addr >= audio_end_addr_list[current_audio])
			{
				AMP_DISABLE();
				usart_data_transmit(USART0, 0x00);
				timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, 0);
				nvic_irq_disable(TIMER5_DAC_IRQn);
				play_flag = 0;

				LED_OFF();
			}
		}
	}
	else if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
	{
		current_audio = (uint8_t)usart_data_receive(USART0);
		
		play_flag = 1;
		
		LED_ON();
		
		pwm_data_addr = audio_start_addr_list[current_audio];
		
		spi_flash_buffer_read(pwm_buf_1, pwm_data_addr, PWM_BUF_LEN);
		pwm_data_addr += PWM_BUF_LEN;
		
		spi_flash_buffer_read(pwm_buf_2, pwm_data_addr, PWM_BUF_LEN);
		pwm_data_addr += PWM_BUF_LEN;
		
		pwm_cnt = 0;
		pwm_refresh_flag = 0;
		pwm_buf = pwm_buf_1;
		
		AMP_ENABLE();
		delay_1ms(500);
		
		timer_counter_value_config(TIMER5, 0);
		timer_flag_clear(TIMER5, TIMER_FLAG_UP);
		nvic_irq_enable(TIMER5_DAC_IRQn, 0, 0);
	}
}
