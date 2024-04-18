/*!
    \file    readme.txt
    \brief   description of audio demo

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

/*
audio player demo using uart to download audio files
USB to UART converter: CH340

Usage:
1、打开pwm_dac.py，在UART_PORT处填入CH340对应的串口号，然后保存运行pwm_dac.py
2、点击“生成文件信息”，python程序开始将audio_file文件夹内的所有文件转换为需要写入的数据
3、生成结束后，audio_table.h被更新，音频文件被编号，同时pwm_dac.py的同级路径下出现audio.bin文件
4、重新编译keil工程，下载工程到MCU
5、MCU开始运行程序后，板载LED亮起表示正在擦除板载SPI FLASH，擦除完成后板载LED熄灭
6、点击“发送音频文件”，python程序控制串口把处理完成的数据发送到MCU，MCU把数据写入到SPI FLASH
7、观察进度条，等待发送完成
8、按板载按键KEY3，LED亮起800ms后熄灭，MCU进入播放模式
9、按照audio_table.h内的音频文件编号，在“音频编号”的文本框内填入需要播放的编号，开始播放
*/
