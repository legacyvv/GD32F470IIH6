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
#include "image1.h"
#include "image2.h"
#include "image3.h"
#include "image4.h"
#include "image5.h"
#include "image6.h"
#include "image7.h"
#include "image8.h"
#include "image9.h"
#include "image10.h"
#include "image11.h"
#include "image12.h"
#include "logo.h"

#define HORIZONTAL_SYNCHRONOUS_PULSE 	4
#define HORIZONTAL_BACK_PORCH 			8
#define ACTIVE_WIDTH 					800
#define HORIZONTAL_FRONT_PORCH 			8

#define VERTICAL_SYNCHRONOUS_PULSE 		4
#define VERTICAL_BACK_PORCH 			16
#define ACTIVE_HEIGHT 					480
#define VERTICAL_FRONT_PORCH 			16

#define FRAME_INTERVAL 					3	

uint8_t blended_address_buffer[58292] __attribute__((at(0xC0000000))); 

uint32_t video_addr[12];
static void ipa_config(uint32_t baseaddress);
static void tli_gpio_config(void);

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
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF;
    tli_init(&tli_init_struct);

    /* TLI layer configuration */
    tli_layer_init_struct.layer_window_leftpos = 80 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_rightpos = (80 + 320 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = 150 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_bottompos = (150 + 100 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    tli_layer_init_struct.layer_sa = 0xFF;
    tli_layer_init_struct.layer_default_blue = 0xFF;
    tli_layer_init_struct.layer_default_green = 0xFF;
    tli_layer_init_struct.layer_default_red = 0xFF;
    tli_layer_init_struct.layer_default_alpha = 0x0;
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)gImage_logo;
    tli_layer_init_struct.layer_frame_line_length = ((320 * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (320 * 2);
    tli_layer_init_struct.layer_frame_total_line_number = 100;
    tli_layer_init(LAYER0, &tli_layer_init_struct);
	
	/* TLI layer1 configuration */
    tli_layer_init_struct.layer_window_leftpos = 80 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_rightpos = (80 + 247 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = 20 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_bottompos = 20 + 118 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    tli_layer_init_struct.layer_sa = 0xFF;
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    tli_layer_init_struct.layer_default_alpha = 0;
    tli_layer_init_struct.layer_default_blue = 0;
    tli_layer_init_struct.layer_default_green = 0;
    tli_layer_init_struct.layer_default_red = 0;
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)&blended_address_buffer;
    tli_layer_init_struct.layer_frame_line_length = ((247 * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (247 * 2);
    tli_layer_init_struct.layer_frame_total_line_number = 118;
    tli_layer_init(LAYER1, &tli_layer_init_struct);
	
	/* enable TLI layers */
    tli_layer_enable(LAYER0);
    tli_layer_enable(LAYER1);
	
	/* enable TLI */
    tli_enable();
	
	/* enable TLI request */
	tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
	tli_reload_config(TLI_REQUEST_RELOAD_EN);
	
}

void ipa_set_transfer_src(uint32_t baseaddress)
{
    IPA_FMADDR = baseaddress;
}

void led_init(void)
{	
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOH);
	/* LED (PH7) */
	gpio_mode_set(GPIOH, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);
	gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);
	gpio_bit_set(GPIOH, GPIO_PIN_7);
}

void lcd_backlight_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
	/*Configure PB5(TIMER2_CH1) as alternate function*/
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

/*!
    \brief      main program
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	uint8_t i = 0, j = 0;
	
    /* configure the SysTick */
    systick_config();
	
	/* configure the EXMC access mode */
	exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
	
	/* configure the TLI */
	display_init();
	
	/* configure the LED */
	led_init();
	
	/* configure the LCD backlight */
	lcd_backlight_gpio_config();
	lcd_backlight_pwm_config();
	
	/* configure the IPA */
	ipa_config((uint32_t)&gImage_image1);
	
	video_addr[0] = (uint32_t)&gImage_image1;
	video_addr[1] = (uint32_t)&gImage_image2;
	video_addr[2] = (uint32_t)&gImage_image3;
	video_addr[3] = (uint32_t)&gImage_image4;
	video_addr[4] = (uint32_t)&gImage_image5;
	video_addr[5] = (uint32_t)&gImage_image6;
	video_addr[6] = (uint32_t)&gImage_image7;
	video_addr[7] = (uint32_t)&gImage_image8;
	video_addr[8] = (uint32_t)&gImage_image9;
	video_addr[9] = (uint32_t)&gImage_image10;
	video_addr[10] = (uint32_t)&gImage_image11;
	video_addr[11] = (uint32_t)&gImage_image12;
	
	while(tli_flag_get(TLI_FLAG_VDE) == 0);
	while(tli_flag_get(TLI_FLAG_VDE) == 1);
	while(tli_flag_get(TLI_FLAG_VDE) == 0);
	
	while(1)
	{
		for(i = 0; i < 12; i++)
		{
			ipa_set_transfer_src(video_addr[i]);
			ipa_transfer_enable();
			while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
			for(j = 0; j < FRAME_INTERVAL; j++)
			{
				while(tli_flag_get(TLI_FLAG_VDE) == 1);
				while(tli_flag_get(TLI_FLAG_VDE) == 0);
			}
		}
	}
}

/*!
    \brief      IPA initialize and configuration
    \param[in]  baseaddress: base address
    \param[out] none
    \retval     none
*/
static void ipa_config(uint32_t baseaddress)
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
    ipa_destination_init_struct.destination_memaddr = (uint32_t)&blended_address_buffer;
    /* configure destination pre-defined alpha value RGB */
    ipa_destination_init_struct.destination_pregreen = 0;
    ipa_destination_init_struct.destination_preblue = 0;
    ipa_destination_init_struct.destination_prered = 0;
    ipa_destination_init_struct.destination_prealpha = 0;
    /* configure destination line offset */
    ipa_destination_init_struct.destination_lineoff = 0;
    /* configure height of the image to be processed */
    ipa_destination_init_struct.image_height = 118;
    /* configure width of the image to be processed */
    ipa_destination_init_struct.image_width = 247;
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
}

/*!
    \brief      configure TLI GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tli_gpio_config(void)
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
	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);
	gpio_bit_set(GPIOD, GPIO_PIN_7);
}
