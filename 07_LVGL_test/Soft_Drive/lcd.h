/*!
    \file    lcd.h
    \brief   the header file of LCD driver

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

#ifndef LCD_H
#define LCD_H

#include "gd32f4xx.h"

#define HORIZONTAL_SYNCHRONOUS_PULSE 	4
#define HORIZONTAL_BACK_PORCH 			8
#define ACTIVE_WIDTH 					800
#define HORIZONTAL_FRONT_PORCH 			8

#define VERTICAL_SYNCHRONOUS_PULSE 		8
#define VERTICAL_BACK_PORCH 			16
#define ACTIVE_HEIGHT 					480
#define VERTICAL_FRONT_PORCH 			16

#define CANVAS_H_OFFSET 				64
#define CANVAS_V_OFFSET 				8
#define CANVAS_H 						640
#define CANVAS_V 						448

#define LCD_BUF_LEN 					(CANVAS_H * CANVAS_V)

extern uint16_t lcd_tli_buffer[CANVAS_H * CANVAS_V];
extern void tli_gpio_config(void);
extern void clear_pixel(uint16_t color);
extern void display_init(void);
extern void ipa_config(uint32_t baseaddress);
extern void ipa_trig(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t src, uint32_t dst);

#endif
