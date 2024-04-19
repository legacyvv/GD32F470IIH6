#include "gd32f4xx.h"

#define AUDIO_NUM 6

const uint32_t audio_start_addr_list[AUDIO_NUM] = {
0,		/*0	不支持特殊符号.wav*/
29184,	/*1	串口发送成功.wav*/
55040,	/*2	为当前配置项添加名称.wav*/
95488,	/*3	扫码领红包.wav*/
119552,	/*4	数据量过大，请停顿5秒.wav*/
160512,	/*5	语音转文字.wav*/
};

const uint32_t audio_end_addr_list[AUDIO_NUM] = {
29184,	/*0	不支持特殊符号.wav*/
55040,	/*1	串口发送成功.wav*/
95488,	/*2	为当前配置项添加名称.wav*/
119552,	/*3	扫码领红包.wav*/
160512,	/*4	数据量过大，请停顿5秒.wav*/
182784,	/*5	语音转文字.wav*/
};