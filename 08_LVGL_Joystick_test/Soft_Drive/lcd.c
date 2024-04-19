/*!
    \file    lcd.c
    \brief   lcd driver

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

pointer_coordinate_struct pointer_coordinate = {100, 380};

//uint16_t lcd_tli_buffer[CANVAS_H * CANVAS_V] __attribute__((at(0xC0000000)));
uint16_t lcd_tli_buffer[CANVAS_H * CANVAS_V] __attribute__ ((section(".ARM.__at_0xC0000000")));

/*!
    \brief      configure TLI GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tli_gpio_config(void)
{
    /* GPIO clock enable */
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
	rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);
	rcu_periph_clock_enable(RCU_GPIOI);

	/*
	HS:PC6  VS:PA4  DE:PF10 CLK:PG7
	R0:PH2  R1:PH3  R2:PC10 R3:PB0  R4:PA11 R5:PA12 R6:PA8  R7:PG6
	G0:PE5  G1:PE6  G2:PA6  G3:PG10 G4:PB10 G5:PB11 G6:PC7  G7:PD3
	B0:PE4  B1:PG12 B2:PD6  B3:PG11 B4:PI4  B5:PI5  B6:PB8  B7:PB9
	*/
	
    /* TLI pins AF configure */
	
	/* CTRL configure */
	gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_6);
	gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_4);
	gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_7);
	gpio_af_set(GPIOF, GPIO_AF_14, GPIO_PIN_10);
	/* R configure */
	gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_2);
	gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_3);
	gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_10);
	gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_0);
	gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_11);
	gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_12);
	gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_8);
	gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_6);
	/* G configure */
	gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_5);
	gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_6);
	gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_6);
	gpio_af_set(GPIOG, GPIO_AF_9, GPIO_PIN_10);
	gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_10);
	gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_11);
	gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_7);
	gpio_af_set(GPIOD, GPIO_AF_14, GPIO_PIN_3);
	/* B configure */
	gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_4);
	gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_12);
	gpio_af_set(GPIOD, GPIO_AF_14, GPIO_PIN_6);
	gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_11);
	gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_4);
	gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_5);
	gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_8);
	gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_9);
	
	/* configure TLI GPIO */
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
		GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12);

	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO_PIN_0 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
		GPIO_PIN_0 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);

	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
		GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10);

	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO_PIN_3 | GPIO_PIN_6);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
		GPIO_PIN_3 | GPIO_PIN_6);

	gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
		GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

	gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, 
		GPIO_PIN_10);
	gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, 
		GPIO_PIN_10);

	gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
	gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
		GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

	gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO_PIN_2 | GPIO_PIN_3);
	gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
		GPIO_PIN_2 | GPIO_PIN_3);

	gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO_PIN_4 | GPIO_PIN_5);
	gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
		GPIO_PIN_4 | GPIO_PIN_5);
	
	/* LCD EN(PD7) */
	gpio_bit_set(GPIOD, GPIO_PIN_7);
	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);
}


void lcd_backlight_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
	/* Configure PB5(TIMER2_CH1) as alternate function */
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_5);
	gpio_af_set(GPIOB, GPIO_AF_2, GPIO_PIN_5);
}

void lcd_backlight_pwm_config(void)
{
	/* TIMER2 configuration: generate PWM signals with different duty cycles */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER2);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);

    timer_deinit(TIMER2);

    /* TIMER2 configuration */
    timer_initpara.prescaler         = 12 - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1000 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);

    /* CH1 configuration in PWM mode 0 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER2,TIMER_CH_1,&timer_ocintpara);

    /* CH1 configuration in PWM mode 0 */
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1,1000);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);
    /* TIMER2 enable */
    timer_enable(TIMER2);
}

const uint16_t pointer_img[POINTER_DIA][POINTER_DIA] = {
	0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
	0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
};

void clear_pixel(uint16_t color)
{
	uint32_t i = 0;
	for(i = 0; i < LCD_BUF_LEN; i++)
	{
		lcd_tli_buffer[i] = color;
	}
}

void display_init(void)
{
	tli_parameter_struct               tli_init_struct;
    tli_layer_parameter_struct         tli_layer_init_struct;

    rcu_periph_clock_enable(RCU_TLI);
    tli_gpio_config();
	
    /* configure the PLLSAI clock to generate lcd clock */
	//CLK = 150 / 3 / 2 = 25MHz
	if(ERROR == rcu_pllsai_config(150, 2, 3)) {
        while(1);
    }
	
	rcu_tli_clock_div_config(RCU_PLLSAIR_DIV2);
    rcu_osci_on(RCU_PLLSAI_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK)) {
        while(1);
    }
	
    /* configure TLI parameter struct */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;

    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;
	
    /* configure LCD background R,G,B values */
    tli_init_struct.backcolor_red = 0x00;
    tli_init_struct.backcolor_green = 0x00;
    tli_init_struct.backcolor_blue = 0x00;
    tli_init(&tli_init_struct);
	
	/* TLI layer0 configuration */
	tli_layer_init_struct.layer_window_leftpos = CANVAS_H_OFFSET + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
	tli_layer_init_struct.layer_window_rightpos = CANVAS_H_OFFSET + CANVAS_H + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
	tli_layer_init_struct.layer_window_toppos = CANVAS_V_OFFSET + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
	tli_layer_init_struct.layer_window_bottompos = CANVAS_V_OFFSET + CANVAS_V + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
	tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
	tli_layer_init_struct.layer_sa = 0xFF;
	tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
	tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
	tli_layer_init_struct.layer_default_alpha = 0;
	tli_layer_init_struct.layer_default_blue = 0;
	tli_layer_init_struct.layer_default_green = 0;
	tli_layer_init_struct.layer_default_red = 0;
	tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)&lcd_tli_buffer;
	tli_layer_init_struct.layer_frame_line_length = ((CANVAS_H * 2) + 3);
	tli_layer_init_struct.layer_frame_buf_stride_offset = (CANVAS_H * 2);
	tli_layer_init_struct.layer_frame_total_line_number = CANVAS_V;
	
	tli_layer_init(LAYER0, &tli_layer_init_struct);
	
	/* TLI layer1 configuration */
	tli_layer_init_struct.layer_window_leftpos = pointer_coordinate.x  - POINTER_RADIUS + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
	tli_layer_init_struct.layer_window_rightpos = pointer_coordinate.x + POINTER_RADIUS + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
	tli_layer_init_struct.layer_window_toppos = pointer_coordinate.y + POINTER_RADIUS + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
	tli_layer_init_struct.layer_window_bottompos = pointer_coordinate.y + POINTER_RADIUS + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
	tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
	tli_layer_init_struct.layer_sa = 0xFF;
	tli_layer_init_struct.layer_default_blue = 0xFF;
	tli_layer_init_struct.layer_default_green = 0xFF;
	tli_layer_init_struct.layer_default_red = 0xFF;
	tli_layer_init_struct.layer_default_alpha = 0x0;
	tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
	tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
	tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)pointer_img;
	tli_layer_init_struct.layer_frame_line_length = ((POINTER_DIA * 2) + 3);
	tli_layer_init_struct.layer_frame_buf_stride_offset = (POINTER_DIA * 2);
	tli_layer_init_struct.layer_frame_total_line_number = POINTER_DIA;
	tli_layer_init(LAYER1, &tli_layer_init_struct);
	
	/* enable TLI layers */
	tli_layer_enable(LAYER0);
	tli_layer_enable(LAYER1);
	
	tli_line_mark_set(ACTIVE_HEIGHT - POINTER_RADIUS);
	
	/* enable TLI */
    tli_enable();
	
	/* enable TLI request */
	tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
	tli_reload_config(TLI_REQUEST_RELOAD_EN);
	
	tli_interrupt_enable(TLI_INT_LM);
	
	nvic_irq_enable(TLI_IRQn, 0U, 0U);
}

/*!
    \brief      IPA initialize and configuration
    \param[in]  baseaddress: base address
    \param[out] none
    \retval     none
*/
void ipa_config(uint32_t baseaddress)
{
    ipa_destination_parameter_struct  ipa_destination_init_struct;
    ipa_foreground_parameter_struct   ipa_fg_init_struct;

    rcu_periph_clock_enable(RCU_IPA);

    ipa_deinit();
    /* configure IPA pixel format convert mode */
    ipa_pixel_format_convert_mode_set(IPA_FGTODE);
    /* configure destination pixel format */
    ipa_destination_init_struct.destination_pf = IPA_DPF_RGB565;
    /* configure destination memory base address */
    ipa_destination_init_struct.destination_memaddr = (uint32_t)&lcd_tli_buffer;
    /* configure destination pre-defined alpha value RGB */
    ipa_destination_init_struct.destination_pregreen = 0;
    ipa_destination_init_struct.destination_preblue = 0;
    ipa_destination_init_struct.destination_prered = 0;
    ipa_destination_init_struct.destination_prealpha = 0;
    /* configure destination line offset */
    ipa_destination_init_struct.destination_lineoff = 0;
    /* configure height of the image to be processed */
    ipa_destination_init_struct.image_height = 0;
    /* configure width of the image to be processed */
    ipa_destination_init_struct.image_width = 0;
    /* IPA destination initialization */
    ipa_destination_init(&ipa_destination_init_struct);

    /* configure IPA foreground */
    ipa_fg_init_struct.foreground_memaddr = baseaddress;
    ipa_fg_init_struct.foreground_pf = FOREGROUND_PPF_RGB565;
    ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;
    ipa_fg_init_struct.foreground_prealpha = 0x0;
    ipa_fg_init_struct.foreground_lineoff = 0x0;
    ipa_fg_init_struct.foreground_preblue = 0x0;
    ipa_fg_init_struct.foreground_pregreen = 0x0;
    ipa_fg_init_struct.foreground_prered = 0x0;
    /* foreground initialization */
    ipa_foreground_init(&ipa_fg_init_struct);
	
	ipa_interrupt_enable(IPA_CTL_FTFIE);
	
	nvic_irq_enable(IPA_IRQn, 1U, 0U);
}

void ipa_trig(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t src, uint32_t dst)
{
	/* foreground memory base address configuration */
	IPA_FMADDR = src;
	/* configure destination memory base address */
	IPA_DMADDR = dst + (CANVAS_H * sy + sx) * 2;
	/* configure destination line offset */
	IPA_DLOFF = CANVAS_H - (ex - sx + 1);
	/* configure width and height of the image to be processed */
	IPA_IMS = ((ex - sx + 1) << 16U) | (ey - sy + 1);
	/* start transfer */
	ipa_transfer_enable();
}

