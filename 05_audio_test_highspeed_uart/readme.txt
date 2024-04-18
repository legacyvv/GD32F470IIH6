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
audio player demo using highspeed uart to download audio files
USB to UART converter: CH343
Baudrate: 6Mbps
TX: PE1
RX: PE0

Usage:
1����pwm_dac.py����UART_PORT������CH343��Ӧ�Ĵ��ںţ�Ȼ�󱣴�����pwm_dac.py
2������������ļ���Ϣ����python����ʼ��audio_file�ļ����ڵ������ļ�ת��Ϊ��Ҫд�������
3�����ɽ�����audio_table.h�����£���Ƶ�ļ�����ţ�ͬʱpwm_dac.py��ͬ��·���³���audio.bin�ļ�
4�����±���keil���̣����ع��̵�MCU
5��MCU��ʼ���г���󣬰���LED�����ʾ���ڲ�������SPI FLASH��������ɺ����LEDϨ��
6�������������Ƶ�ļ�����python������ƴ��ڰѴ�����ɵ����ݷ��͵�MCU��MCU������д�뵽SPI FLASH
7���۲���������ȴ��������
8�������ذ���KEY3��LED����800ms��Ϩ��MCU���벥��ģʽ
9������audio_table.h�ڵ���Ƶ�ļ���ţ��ڡ���Ƶ��š����ı�����������Ҫ���ŵı�ţ���ʼ����
*/
