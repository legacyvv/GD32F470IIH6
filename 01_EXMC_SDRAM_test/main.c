/*!
    \file    main.c
    \brief   EXMC SDRAM demo

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

/* SDRAM */
#define TOTAL_SIZE 				(8 * 1024 * 1024)
uint32_t writereadstatus = 0;
uint32_t temp_addr;

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
}

void led_init(void)
{
	/*
	LED: PH7
	*/
	
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOH);
	/* LED (PH7) */
	gpio_mode_set(GPIOH, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);
	gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);
	gpio_bit_set(GPIOH, GPIO_PIN_7);
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

	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI4_IRQn, 2U, 0U);
	nvic_irq_enable(EXTI0_IRQn, 2U, 0U);
	nvic_irq_enable(EXTI2_IRQn, 2U, 0U);

	/* connect key EXTI line to key GPIO pin */
	syscfg_exti_line_config(EXTI_SOURCE_GPIOH, EXTI_SOURCE_PIN4);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN0);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOG, EXTI_SOURCE_PIN2);
	
	/* configure key EXTI line */
	exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_flag_clear(EXTI_4);
	exti_interrupt_flag_clear(EXTI_0);
	exti_interrupt_flag_clear(EXTI_2);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	uint32_t i = 0, write_data = 0, read_data = 0;
	uint8_t temp = 0, data_check = 0;

	/* initialize LED */
	led_init();

	/* initialize KEY */
	key_init();

	/* configure systick clock */
	systick_config();

	/* configure the USART */
	usart0_init();

	/* configure the EXMC access mode */
	exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);

	printf("\r\nSDRAM initialized!");
	
	/* scan SDRAM */
	temp_addr = SDRAM_DEVICE0_ADDR;
	for(i = 0; i < TOTAL_SIZE; i++)
	{
		temp = i & 0xff;
		write_data = ((temp << 24) | (temp << 16) | (temp << 8) | temp);
		*(uint32_t *)temp_addr = write_data;
		read_data = *(uint32_t *)temp_addr;
		if((temp_addr & 0xffff) == 0)
		{
			printf("\r\n0x%8x", temp_addr);
		}
		if(write_data == read_data)
		{
			temp_addr += 4;
		}
		else
		{
			data_check = 1;
			break;
		}
	}
	
	if(data_check) {
		printf("\r\nSDRAM test failed!");
	}
	else
	{
		printf("\r\nSDRAM test successed!");
	}
	
	while(1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
