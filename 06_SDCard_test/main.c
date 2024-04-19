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
#include "sdcard.h"
#include "exmc_sdram.h"

sd_card_info_struct sd_cardinfo;                            /* information of SD card */
uint32_t buf_write[512];                                    /* store the data written to the card */
uint32_t buf_read[512];                                     /* store the data read from the card */

void nvic_config(void);
sd_error_enum sd_config(void);
void card_info_get(void);

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
	sd_error_enum sd_error;
    uint16_t i = 5;

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

	nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(SDIO_IRQn, 0, 0);
	
	do {
        /* initialize the card, get the card information and configurate the bus mode and transfer mode */
        sd_error = sd_config();
    } while((SD_OK != sd_error) && (--i));

    if(i) {
        printf("\r\n Card init success!\r\n");
    } else {
        printf("\r\n Card init failed!\r\n");
        while(1) {
        }
    }
	
    /* get the information of the card and print it out by USART */
    card_info_get();

    /* init the write buffer */
    for(i = 0; i < 512; i++) {
        buf_write[i] = i;
    }
    printf("\r\n\r\n Card test:");
	
	   /* single block operation test */
    sd_error = sd_block_write(buf_write, 100 * 512, 512);
    if(SD_OK != sd_error) {
        printf("\r\n Block write fail!");
        while(1) {
        }
    } else {
        printf("\r\n Block write success!");
    }
    sd_error = sd_block_read(buf_read, 100 * 512, 512);
	
	if(SD_OK != sd_error) {
        printf("\r\n Block read fail!");
        while(1) {
        }
    } else {
        printf("\r\n Block read success!");
#ifdef DATA_PRINT
        pdata = (uint8_t *)buf_read;
        /* print data by USART */
        printf("\r\n");
        for(i = 0; i < 128; i++) {
            printf(" %3d %3d %3d %3d ", *pdata, *(pdata + 1), *(pdata + 2), *(pdata + 3));
            pdata += 4;
            if(0 == (i + 1) % 4) {
                printf("\r\n");
            }
        }
#endif /* DATA_PRINT */
    }

    /* lock and unlock operation test */
    if(SD_CCC_LOCK_CARD & sd_cardinfo.card_csd.ccc) {
        /* lock the card */
        sd_error = sd_lock_unlock(SD_LOCK);
        if(SD_OK != sd_error) {
            printf("\r\n Lock failed!");
            while(1) {
            }
        } else {
            printf("\r\n The card is locked!");
        }
        sd_error = sd_erase(100 * 512, 101 * 512);
        if(SD_OK != sd_error) {
            printf("\r\n Erase failed!");
        } else {
            printf("\r\n Erase success!");
        }

        /* unlock the card */
        sd_error = sd_lock_unlock(SD_UNLOCK);
        if(SD_OK != sd_error) {
            printf("\r\n Unlock failed!");
            while(1) {
            }
        } else {
            printf("\r\n The card is unlocked!");
        }
        sd_error = sd_erase(100 * 512, 101 * 512);
        if(SD_OK != sd_error) {
            printf("\r\n Erase failed!");
        } else {
            printf("\r\n Erase success!");
        }

        sd_error = sd_block_read(buf_read, 100 * 512, 512);
        if(SD_OK != sd_error) {
            printf("\r\n Block read fail!");
            while(1) {
            }
        } else {
            printf("\r\n Block read success!");
#ifdef DATA_PRINT
            pdata = (uint8_t *)buf_read;
            /* print data by USART */
            printf("\r\n");
            for(i = 0; i < 128; i++) {
                printf(" %3d %3d %3d %3d ", *pdata, *(pdata + 1), *(pdata + 2), *(pdata + 3));
                pdata += 4;
                if(0 == (i + 1) % 4) {
                    printf("\r\n");
                }
            }
#endif /* DATA_PRINT */
        }
    }

    /* multiple blocks operation test */
    sd_error = sd_multiblocks_write(buf_write, 200 * 512, 512, 3);
    if(SD_OK != sd_error) {
        printf("\r\n Multiple block write fail!");
        while(1) {
        }
    } else {
        printf("\r\n Multiple block write success!");
    }
    sd_error = sd_multiblocks_read(buf_read, 200 * 512, 512, 3);
    if(SD_OK != sd_error) {
        printf("\r\n Multiple block read fail!");
        while(1) {
        }
    } else {
        printf("\r\n Multiple block read success!");
#ifdef DATA_PRINT
        pdata = (uint8_t *)buf_read;
        /* print data by USART */
        printf("\r\n");
        for(i = 0; i < 512; i++) {
            printf(" %3d %3d %3d %3d ", *pdata, *(pdata + 1), *(pdata + 2), *(pdata + 3));
            pdata += 4;
            if(0 == (i + 1) % 4) {
                printf("\r\n");
            }
        }
#endif /* DATA_PRINT */
    }
	
	while(1);
}

/*!
    \brief      initialize the card, get the card information, set the bus mode and transfer mode
    \param[in]  none
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_config(void)
{
    sd_error_enum status = SD_OK;
    uint32_t cardstate = 0;
    /* initialize the card */
    status = sd_init();
    if(SD_OK == status) {
        status = sd_card_information_get(&sd_cardinfo);
    }
    if(SD_OK == status) {
        status = sd_card_select_deselect(sd_cardinfo.card_rca);
    }
    status = sd_cardstatus_get(&cardstate);
    if(cardstate & 0x02000000) {
        printf("\r\n The card is locked!");
#if 0
        /* unlock the card if necessary */
        status = sd_lock_unlock(SD_UNLOCK);
        if(SD_OK != status) {
            printf("\r\n Unlock failed!");
            while(1) {
            }
        } else {
            printf("\r\n The card is unlocked! Please reset MCU!");
        }
#endif
        while(1) {
        }
    }
    if((SD_OK == status) && (!(cardstate & 0x02000000))) {
        /* set bus mode */
        status = sd_bus_mode_config(SDIO_BUSMODE_4BIT);
//        status = sd_bus_mode_config( SDIO_BUSMODE_1BIT );
    }
    if(SD_OK == status) {
        /* set data transfer mode */
//        status = sd_transfer_mode_config( SD_DMA_MODE );
        status = sd_transfer_mode_config(SD_POLLING_MODE);
    }
    return status;
}

/*!
    \brief      get the card information and print it out by USRAT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void card_info_get(void)
{
    uint8_t sd_spec, sd_spec3, sd_spec4, sd_security;
    uint32_t block_count, block_size;
    uint16_t temp_ccc;
    printf("\r\n Card information:");
    sd_spec = (sd_scr[1] & 0x0F000000) >> 24;
    sd_spec3 = (sd_scr[1] & 0x00008000) >> 15;
    sd_spec4 = (sd_scr[1] & 0x00000400) >> 10;
    if(2 == sd_spec) {
        if(1 == sd_spec3) {
            if(1 == sd_spec4) {
                printf("\r\n## Card version 4.xx ##");
            } else {
                printf("\r\n## Card version 3.0x ##");
            }
        } else {
            printf("\r\n## Card version 2.00 ##");
        }
    } else if(1 == sd_spec) {
        printf("\r\n## Card version 1.10 ##");
    } else if(0 == sd_spec) {
        printf("\r\n## Card version 1.0x ##");
    }

    sd_security = (sd_scr[1] & 0x00700000) >> 20;
    if(2 == sd_security) {
        printf("\r\n## SDSC card ##");
    } else if(3 == sd_security) {
        printf("\r\n## SDHC card ##");
    } else if(4 == sd_security) {
        printf("\r\n## SDXC card ##");
    }

    block_count = (sd_cardinfo.card_csd.c_size + 1) * 1024;
    block_size = 512;
    printf("\r\n## Device size is %dKB ##", sd_card_capacity_get());
    printf("\r\n## Block size is %dB ##", block_size);
    printf("\r\n## Block count is %d ##", block_count);

    if(sd_cardinfo.card_csd.read_bl_partial) {
        printf("\r\n## Partial blocks for read allowed ##");
    }
    if(sd_cardinfo.card_csd.write_bl_partial) {
        printf("\r\n## Partial blocks for write allowed ##");
    }
    temp_ccc = sd_cardinfo.card_csd.ccc;
    printf("\r\n## CardCommandClasses is: %x ##", temp_ccc);
    if((SD_CCC_BLOCK_READ & temp_ccc) && (SD_CCC_BLOCK_WRITE & temp_ccc)) {
        printf("\r\n## Block operation supported ##");
    }
    if(SD_CCC_ERASE & temp_ccc) {
        printf("\r\n## Erase supported ##");
    }
    if(SD_CCC_WRITE_PROTECTION & temp_ccc) {
        printf("\r\n## Write protection supported ##");
    }
    if(SD_CCC_LOCK_CARD & temp_ccc) {
        printf("\r\n## Lock unlock supported ##");
    }
    if(SD_CCC_APPLICATION_SPECIFIC & temp_ccc) {
        printf("\r\n## Application specific supported ##");
    }
    if(SD_CCC_IO_MODE & temp_ccc) {
        printf("\r\n## I/O mode supported ##");
    }
    if(SD_CCC_SWITCH & temp_ccc) {
        printf("\r\n## Switch function supported ##");
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
