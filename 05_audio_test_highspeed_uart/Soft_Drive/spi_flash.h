/*!
    \file    spi_flash.h
    \brief   the header file of SPI flash driver

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

#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#include "gd32f4xx.h"

#define F_SPI 			SPI4

#define SPI_FLASH_PAGE_SIZE 	0x100
#define SPI_FLASH_CS_LOW() 		gpio_bit_reset(GPIOF,GPIO_PIN_6)
#define SPI_FLASH_CS_HIGH() 	gpio_bit_set(GPIOF,GPIO_PIN_6)

#define WRITE 			0x02     /* write to memory instruction */
#define QUADWRITE 		0x32     /* quad write to memory instruction */
#define WRSR 			0x01     /* write status register instruction */
#define WREN 			0x06     /* write enable instruction */

#define READ 			0x03     /* read from memory instruction */
#define QUADREAD 		0x6B     /* read from memory instruction */
#define RDSR 			0x05     /* read status register instruction */
#define RDID 			0x9F     /* read identification */
#define SE 				0x20     /* sector erase instruction */
#define BE 				0xC7     /* bulk erase instruction */

#define WTSR 			0x05     /* write status register instruction */

#define WIP_FLAG 		0x01     /* write in progress(wip) flag */
#define DUMMY_BYTE 		0xA5

/* initialize SPI GPIO and parameter */
extern void spi_flash_init(void);

/* send a byte through the SPI interface and return the byte received from the SPI bus */
extern uint8_t spi_flash_send_byte(uint8_t byte);

/* read a byte from the SPI flash */
extern uint8_t spi_flash_read_byte(void);

/* enable the write access to the flash */
extern void spi_flash_write_enable(void);

/* poll the status of the write in progress (wip) flag in the flash's status register */
extern void spi_flash_wait_for_write_end(void);

/* erase the specified flash sector */
extern void spi_flash_sector_erase(uint32_t sector_addr);

/* erase the entire flash */
extern void spi_flash_bulk_erase(void);

/* write more than one byte to the flash */
extern void spi_flash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write);

/* read a block of data from the flash */
extern void spi_flash_buffer_read(uint8_t *pbuffer, uint32_t read_addr, uint16_t num_byte_to_read);

/* read flash identification */
extern uint32_t spi_flash_read_id(void);

#endif /* SPI_FLASH_H */
