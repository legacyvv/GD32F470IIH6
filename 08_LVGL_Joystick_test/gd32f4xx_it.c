/*!
    \file    gd32f4xx_it.c
    \brief   interrupt service routines

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

#include "gd32f4xx_it.h"
#include "systick.h"
#include "stdio.h"
#include "lvgl.h"
#include "lcd.h"
#include "joystick.h"

//extern lv_disp_drv_t disp_drv;

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
	lv_tick_inc(1);
}

void EXTI4_IRQHandler(void)
{
	/* KEY1 */
    if(RESET != exti_interrupt_flag_get(EXTI_4)) {
        gpio_bit_toggle(GPIOH, GPIO_PIN_7);
        exti_interrupt_flag_clear(EXTI_4);
//		if(pointer_x > 8)
//			pointer_x -= 8;
    }
}

void EXTI0_IRQHandler(void)
{
	/* KEY2 */
    if(RESET != exti_interrupt_flag_get(EXTI_0)) {
        gpio_bit_toggle(GPIOH, GPIO_PIN_7);
        exti_interrupt_flag_clear(EXTI_0);
//		if(pointer_x < (ACTIVE_WIDTH - 8))
//			pointer_x += 8;
    }
}

void EXTI2_IRQHandler(void)
{
	/* KEY3 */
    if(RESET != exti_interrupt_flag_get(EXTI_2)) {
        gpio_bit_toggle(GPIOH, GPIO_PIN_7);
        exti_interrupt_flag_clear(EXTI_2);
    }
}

/*!
    \brief      this function handles USART0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART0_IRQHandler(void)
{
	uint8_t i = 0;
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
        /* read one byte from the receive data register */
        i = (uint8_t)usart_data_receive(USART0);
		/* set LCD backlight */
		timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_1, i * 10);
    }
}

void TIMER1_IRQHandler(void)
{
	if(RESET != timer_flag_get(TIMER1, TIMER_FLAG_UP))
	{
		timer_flag_clear(TIMER1, TIMER_FLAG_UP);
		
		//printf("%1d", GET_JOY_KEY());
		
		if((joystick_value.x > (JOY_ADC_FULL - JOY_ADC_THRES)) && (pointer_coordinate.x > POINTER_RADIUS))
			pointer_coordinate.x--;
		if((joystick_value.x < JOY_ADC_THRES) && (pointer_coordinate.x < (ACTIVE_WIDTH - POINTER_RADIUS - 1)))
			pointer_coordinate.x++;
		if((joystick_value.y < JOY_ADC_THRES) && (pointer_coordinate.y > POINTER_RADIUS))
			pointer_coordinate.y--;
		if((joystick_value.y > (JOY_ADC_FULL - JOY_ADC_THRES)) && (pointer_coordinate.y < (ACTIVE_HEIGHT - POINTER_RADIUS - 1)))
			pointer_coordinate.y++;
	}
}

void TLI_IRQHandler(void)
{
	uint16_t l_pos, r_pos, t_pos, b_pos;
	if(RESET != tli_interrupt_flag_get(TLI_INT_FLAG_LM))
	{
		tli_interrupt_flag_clear(TLI_INT_FLAG_LM);
		
		l_pos = pointer_coordinate.x - POINTER_RADIUS + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
		r_pos = pointer_coordinate.x + POINTER_RADIUS + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
		t_pos = pointer_coordinate.y - POINTER_RADIUS + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
		b_pos = pointer_coordinate.y + POINTER_RADIUS + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
		
		TLI_LxHPOS(LAYER1) = l_pos | (r_pos << 16);
		TLI_LxVPOS(LAYER1) = t_pos | (b_pos << 16);
		tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
	}
}

