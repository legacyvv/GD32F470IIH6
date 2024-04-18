/*!
    \file    audio.h
    \brief   the header file of audio driver

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

#ifndef AUDIO_H
#define AUDIO_H

#include "gd32f4xx.h"

#define UART_RECV_LEN 	256
#define PWM_BUF_LEN 	128

#define AMP_ENABLE() 	gpio_bit_set(GPIOG, GPIO_PIN_3);
#define AMP_DISABLE() 	gpio_bit_reset(GPIOG, GPIO_PIN_3);

extern uint8_t uart_recv_buf[UART_RECV_LEN];
extern uint16_t uart_recv_cnt;
extern uint8_t uart_recv_flag;

extern uint8_t *pwm_buf;
extern uint16_t pwm_cnt;
extern uint8_t pwm_refresh_flag;

extern void pwm_dac_gpio_config(void);
extern void pwm_dac_pwm_config(void);
extern void pwm_dac_timer_config(void);
extern void mute_init(void);
extern void erase_audio_data(void);
extern void recv_audio_data_from_uart(void);
extern void play_audio(void);

#endif /* AUDIO_H */
