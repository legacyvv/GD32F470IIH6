/*!
    \file    adc.c
    \brief   adc driver

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
#include "joystick.h"

joystick_value_struct joystick_value;

void adc_gpio_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_mode_set(GPIOC,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_0);
	gpio_mode_set(GPIOC,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_1);
}

void adc_config(void)
{
	/* enable ADC0 clock */
	rcu_periph_clock_enable(RCU_ADC0);
	/* config ADC clock */
	adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
	/* configure the ADC sync mode */
	adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	adc_dma_mode_enable(ADC0);
	adc_dma_request_after_last_enable(ADC0);
	/* ADC data alignment config */
	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
	/* ADC scan mode function enable */
	adc_special_function_config(ADC0, ADC_SCAN_MODE,ENABLE);

	/* ADC channel length config */
	adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 2);
	/* ADC routine channel config */
	adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_480);
	adc_routine_channel_config(ADC0, 1, ADC_CHANNEL_11, ADC_SAMPLETIME_480);
	/* ADC external trigger enable */
	adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_FALLING);
	adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T1_CH1);

	/* enable ADC interface */
	adc_enable(ADC0);
	/* ADC calibration and reset calibration */
	adc_calibration_enable(ADC0);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;
    
	rcu_periph_clock_enable(RCU_DMA1);
	
    /* ADC_DMA_channel deinit */
    dma_deinit(DMA1,DMA_CH0);
    
    /* initialize DMA single data mode */
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(&joystick_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number = 2;
    dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1,DMA_CH0,&dma_single_data_parameter);
    /* DMA channel 0 peripheral select */
    dma_channel_subperipheral_select(DMA1,DMA_CH0,DMA_SUBPERI0);

    /* enable DMA channel */
    dma_channel_enable(DMA1,DMA_CH0);
}

/*!
    \brief      configure the timer peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

	rcu_periph_clock_enable(RCU_TIMER1);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
	
    /* TIMER1 configuration */
    timer_initpara.prescaler         = SystemCoreClock / 1000 / SAMPLE_RATE - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1000 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH1 configuration in PWM mode0 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);
	
	/* sample start at 30% */
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,300 - 1);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
    
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
	
	timer_counter_value_config(TIMER1, 0U);
	timer_flag_clear(TIMER1, TIMER_FLAG_UP);
	nvic_irq_enable(TIMER1_IRQn, 2U, 0U);
	
    /* enable TIMER1 */
    timer_enable(TIMER1);
}

void joystick_button_config(void)
{
	/* enable the key clock */
	rcu_periph_clock_enable(RCU_GPIOI);
	/* configure button pin as input */
	gpio_mode_set(GPIOI, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_8);
}

void joystick_config(void)
{
	adc_gpio_config();
	adc_config();
	adc_dma_config();
	adc_timer_config();
	joystick_button_config();
}

