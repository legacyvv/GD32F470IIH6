/*!
    \file    spi_flash.c
    \brief   SPI flash gd25qxx driver

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

#include "spi_flash.h"
#include "gd32f4xx.h"
#include <string.h>

/*!
    \brief      initialize SPI GPIO and parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_SPI4);

    /* SPI4_CLK(PF7), SPI4_MISO(PF8), SPI4_MOSI(PF9) GPIO pin configuration */
    gpio_af_set(GPIOF, GPIO_AF_5, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);

    /* SPI4_CS(PF6) GPIO pin configuration */
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* chip select invalid */
    SPI_FLASH_CS_HIGH();

    /* SPI4 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI4, &spi_init_struct);

    /* enable SPI4 */
    spi_enable(SPI4);
}

/*!
    \brief      send a byte through the SPI interface and return the byte received from the SPI bus
    \param[in]  byte: byte to send
    \param[out] none
    \retval     the value of the received byte
*/
uint8_t spi_flash_send_byte(uint8_t byte)
{
    /* loop while data register in not emplty */
    while(RESET == spi_i2s_flag_get(F_SPI, SPI_FLAG_TBE));

    /* send byte through the SPI peripheral */
    spi_i2s_data_transmit(F_SPI, byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(F_SPI, SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(F_SPI));
}

/*!
    \brief      read a byte from the SPI flash
    \param[in]  none
    \param[out] none
    \retval     byte read from the SPI flash
*/
uint8_t spi_flash_read_byte(void)
{
    return(spi_flash_send_byte(DUMMY_BYTE));
}

/*!
    \brief      enable the write access to the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_write_enable(void)
{
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "write enable" instruction */
    spi_flash_send_byte(WREN);

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}

/*!
    \brief      poll the status of the write in progress(wip) flag in the flash's status register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_wait_for_write_end(void)
{
    uint8_t flash_status = 0;

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "read status register" instruction */
    spi_flash_send_byte(RDSR);

    /* loop as long as the memory is busy with a write cycle */
    do {
        /* send a dummy byte to generate the clock needed by the flash
        and put the value of the status register in flash_status variable */
        flash_status = spi_flash_send_byte(DUMMY_BYTE);
    } while(SET == (flash_status & WIP_FLAG));

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}

/*!
    \brief      erase the specified flash sector
    \param[in]  sector_addr: address of the sector to erase
    \param[out] none
    \retval     none
*/
void spi_flash_sector_erase(uint32_t sector_addr)
{
    /* send write enable instruction */
    spi_flash_write_enable();

    /* sector erase */
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send sector erase instruction */
    spi_flash_send_byte(SE);
    /* send sector_addr high nibble address byte */
    spi_flash_send_byte((sector_addr & 0xFF0000) >> 16);
    /* send sector_addr medium nibble address byte */
    spi_flash_send_byte((sector_addr & 0xFF00) >> 8);
    /* send sector_addr low nibble address byte */
    spi_flash_send_byte(sector_addr & 0xFF);
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      erase the entire flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_bulk_erase(void)
{
    /* send write enable instruction */
    spi_flash_write_enable();

    /* bulk erase */
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send bulk erase instruction  */
    spi_flash_send_byte(BE);
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      write more than one byte to the flash
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  write_addr: flash's internal address to write
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
void spi_flash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    /* enable the write access to the flash */
    spi_flash_write_enable();

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "write to memory" instruction */
    spi_flash_send_byte(WRITE);
    /* send write_addr high nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF0000) >> 16);
    /* send write_addr medium nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF00) >> 8);
    /* send write_addr low nibble address byte to write to */
    spi_flash_send_byte(write_addr & 0xFF);

    /* while there is data to be written on the flash */
    while(num_byte_to_write--) {
        /* send the current byte */
        spi_flash_send_byte(*pbuffer);
        /* point on the next byte to be written */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      read a block of data from the flash
    \param[in]  pbuffer: pointer to the buffer that receives the data read from the flash
    \param[in]  read_addr: flash's internal address to read from
    \param[in]  num_byte_to_read: number of bytes to read from the flash
    \param[out] none
    \retval     none
*/
void spi_flash_buffer_read(uint8_t *pbuffer, uint32_t read_addr, uint16_t num_byte_to_read)
{
    /* select the flash: chip slect low */
    SPI_FLASH_CS_LOW();

    /* send "read from memory " instruction */
    spi_flash_send_byte(READ);

    /* send read_addr high nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF00) >> 8);
    /* send read_addr low nibble address byte to read from */
    spi_flash_send_byte(read_addr & 0xFF);

    /* while there is data to be read */
    while(num_byte_to_read--) {
        /* read a byte from the flash */
        *pbuffer = spi_flash_send_byte(DUMMY_BYTE);
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}

/*!
    \brief      read flash identification
    \param[in]  none
    \param[out] none
    \retval     flash identification
*/
uint32_t spi_flash_read_id(void)
{
    uint32_t temp = 0, temp0 = 0, temp1 = 0, temp2 = 0;

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "RDID " instruction */
    spi_flash_send_byte(RDID);

    /* read a byte from the flash */
    temp0 = spi_flash_send_byte(DUMMY_BYTE);

    /* read a byte from the flash */
    temp1 = spi_flash_send_byte(DUMMY_BYTE);

    /* read a byte from the flash */
    temp2 = spi_flash_send_byte(DUMMY_BYTE);

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    temp = (temp0 << 16) | (temp1 << 8) | temp2;

    return temp;
}

