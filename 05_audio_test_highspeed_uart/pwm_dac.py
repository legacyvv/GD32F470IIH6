#based on python 3.7.0

import serial #导入模块
import wave
import numpy as np
import sys
import os
import tkinter
from tkinter import ttk
from scipy import interpolate

UART_PORT = "COM9"
baudrate = 6000000
resample_rate = 16000

gain = 1
datalist = [0]
datalist_hex = []
uart_send_len = 256

root = r'.\audio_file'
textfile = r'.\audio_table.h'
path_list = os.listdir(root)
path_list.sort()

print("---------------")

audio_name_list = []
audio_len_list = []
addr_temp = [0]
start_addr_list = [0]
end_addr_list = []
send_num = [0]

audio_bin_data = []
audio_bin_len = [0]

def info_generate():
    audio_name_list.clear()
    audio_len_list.clear()
    addr_temp[0] = 0
    start_addr_list.clear()
    start_addr_list.append(0)
    end_addr_list.clear()
    send_num[0] = 0
    path_list = os.listdir(root)
    path_list.sort()

    if(os.path.isfile('audio.bin')):
        os.remove('audio.bin')
    
    for file in path_list:
        audio_name_list.append(file)
        print(file)

        filepath = os.path.join(root,file)
        f = wave.open(filepath, 'rb' )
        params = f.getparams()
        nchannels, sampwidth, framerate, nframes = params[:4]
         
        #print("声道数：",nchannels)
        #print(sampwidth)
        #print("采样率：",framerate)
        #print(nframes)

        Data_str = f.readframes(nframes)
        Data_num = np.frombuffer(Data_str,dtype=np.int16)

        datalist = list(Data_num)
        f.close()

        #删除头尾无效数据
        while(datalist[-1] == 0):
            del(datalist[-1])

        while(datalist[0] == 0):
            del(datalist[0])

        print("原始数据长度：",len(datalist))
        
        #采样率转换
        if(framerate == resample_rate):
            #pass
            print('无需重采样')
        else:
            print('重采样到',str(resample_rate),'Hz')
            new_len = int(len(datalist) * resample_rate / framerate - 1)
            t = np.linspace(0,100000,len(datalist))
            #print('t len = ',len(t))
            f = interpolate.interp1d(t,datalist,kind = 'quadratic')
            tnew = np.linspace(0,100000,new_len)
            datalist = list(f(tnew))
            print('重采样后长度',str(len(datalist)))
        
        for i in range(len(datalist)):
            datalist[i] = int((datalist[i] * gain + 32768) / 256)
        
        #计算实际写入长度
        if len(datalist)%uart_send_len == 0:
            list_write_len = len(datalist)
            #print('mod 256')
        else:
            list_write_len = int((int(len(datalist)/uart_send_len)+1)*uart_send_len)
            last_send = len(datalist)%uart_send_len
            for i in range(uart_send_len - last_send):
                datalist.append(0)
        audio_len_list.append(list_write_len)

        '''
        last_send = len(datalist)%uart_send_len
        if(last_send == 0):
            pass
        else:
            for i in range(uart_send_len - last_send):
                datalist.append(0)
        '''
        
        with open('audio.bin', 'ab+') as f:
            f.write(bytes(datalist))
        
        print("写入长度：",list_write_len)
        print("")

    #print(audio_len_list)
    for i in range(len(audio_len_list)):
        addr_temp[0] = addr_temp[0] + audio_len_list[i]
        start_addr_list.append(addr_temp[0])
        end_addr_list.append(addr_temp[0])
    print("总大小：",start_addr_list[-1],"bytes ",round(start_addr_list[-1]*8/1024/1024,2),"Mbits")
    del(start_addr_list[-1])

    with open(textfile, 'w+') as outFile:
        
        outFile.write("#include \"gd32f4xx.h\"\n\n")

        outFile.write("#define AUDIO_NUM "+str(len(start_addr_list))+"\n\n")
                
        outFile.write("const uint32_t audio_start_addr_list[AUDIO_NUM] = {\n")
        
        outFile.write("0,\t\t/*"+ '0\t' + audio_name_list[0] +"*/\n")
        
        for i in range(1,len(start_addr_list)):
            outFile.write(str(start_addr_list[i])+",\t/*"+ str(i) + '\t' + audio_name_list[i] +"*/\n")
        outFile.write('};\n\n')

        outFile.write("const uint32_t audio_end_addr_list[AUDIO_NUM] = {\n")
        for i in range(len(end_addr_list)):
            outFile.write(str(end_addr_list[i])+",\t/*"+ str(i) + '\t' + audio_name_list[i] +"*/\n")
        outFile.write('};\r')

    print()
    print("bin文件已生成")
    print("文件信息已生成，请重新编译工程并下载至MCU")
    print()
    #print("MCU程序开始运行后，按回车开始将音频文件下载到SPI FLASH")
    
    send_num[0] = int(end_addr_list[-1]/uart_send_len)
    print('总共需要发送',str(send_num[0]),"次")
    print("---------------")
    text.set('0/'+str(send_num[0]))
    progressbar['maximum']=send_num[0]

send_cnt = [0]

def transfer_progress_refresh():
    send_cnt[0] = send_cnt[0]+1
    progressbar['value']=send_cnt[0]
    text.set(str(send_cnt[0])+'/'+str(send_num[0]))
    label.place(x=152,y=105)
    TK_Proj.update()

def audio_transfer(wavefile):
    filepath = os.path.join(root,wavefile)
    f = wave.open(filepath, 'rb' )
    params = f.getparams()
    nchannels, sampwidth, framerate, nframes = params[:4]

    Data_str = f.readframes(nframes)
    Data_num = np.frombuffer(Data_str,dtype=np.int16)

    datalist = list(Data_num)
    f.close()

    #删除头尾无效数据
    while(datalist[-1] == 0):
        del(datalist[-1])
    
    while(datalist[0] == 0):
        del(datalist[0])
    
    print("原始数据长度：",len(datalist))
    
    cnt = 0
    data_sum = 0
    write_in_num = 0

    #采样率转换
    if(framerate == resample_rate):
        print('无需重采样')
    else:
        print('重采样到',str(resample_rate),'Hz')
        new_len = int(len(datalist) * resample_rate / framerate - 1)
        t = np.linspace(0,100000,len(datalist))
        #print('t len = ',len(t))
        f = interpolate.interp1d(t,datalist,kind = 'quadratic')
        tnew = np.linspace(0,100000,new_len)
        datalist = list(f(tnew))
        print('重采样后长度',str(len(datalist)))
    
    for i in range(len(datalist)):
        datalist[i] = int((datalist[i] * gain + 32768) / 256)

    if len(datalist)%uart_send_len == 0:
        list_write_len = len(datalist)
    else:
        list_write_len = int((int(len(datalist)/uart_send_len)+1)*uart_send_len)
    audio_len_list.append(list_write_len)
    print("写入长度：",list_write_len)

    for i in range(int(len(datalist)/uart_send_len)):
        try:
            portx=UART_PORT
            bps=baudrate
            #超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
            timex=20
            ser=serial.Serial(portx,bps,timeout=timex)
            #print("串口详情参数：", ser)
            result=ser.write(datalist[i*uart_send_len:(i+1)*uart_send_len])
            #print("写总字节数:",result)
            #十六进制的读取
            ser.read(15)
            #print(ser.read(15).decode('utf-8'))

            #print("---------------")
            ser.close()#关闭串口
            
            transfer_progress_refresh()

        except Exception as e:
            print("---异常---：",e)
            break

    last_send = len(datalist)%uart_send_len
    print("last_send")

    datalist_temp = []
    if last_send != 0:
        for i in range(uart_send_len):
            datalist_temp.append(0)
        for i in range(last_send):
            datalist_temp[i] = datalist[len(datalist) - last_send + i]
        
        try:
            portx=UART_PORT
            bps=baudrate
            #超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
            timex=10
            ser=serial.Serial(portx,bps,timeout=timex)
            #print("串口详情参数：", ser)

            result=ser.write(datalist_temp)
            #print("写总字节数:",result)
            #十六进制的读取
            ser.read(15)
            #print(ser.read(15).decode('utf-8'))
            
            ser.close()#关闭串口

            transfer_progress_refresh()

        except Exception as e:
            print("---异常---：",e)
    print("---------------")

def ReadFile():
    audio_bin_data.clear()
    
    filepath='audio.bin'        ## 文件路径引入
    binfile = open(filepath, 'rb')   ## 打开二进制文件
    audio_bin_len[0] = os.path.getsize(filepath)    ## 获得文件大小
    print('总大小：',audio_bin_len[0],'bytes')      ## 打印文件大小
    for i in range(audio_bin_len[0]):               ## 遍历输出文件内容
        audio_bin_data.append(ord(binfile.read(1))) ## 每次输出一个字节        
    binfile.close()                  ## close文件
    #print(audio_bin_data[:100])
    
def transfer_all_file():
    ReadFile()
    print('bin文件已读取，开始发送')
    #print(int(audio_bin_len[0]/uart_send_len))
    send_num[0] = int(audio_bin_len[0]/uart_send_len)
    progressbar['maximum']=int(audio_bin_len[0]/uart_send_len)
    text.set('0/'+str(send_num[0]))
    for i in range(int(audio_bin_len[0]/uart_send_len)):
        try:
            portx=UART_PORT
            bps=baudrate
            #超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
            timex=20
            ser=serial.Serial(portx,bps,timeout=timex)
            #print("串口详情参数：", ser)
            result=ser.write(audio_bin_data[i*uart_send_len:(i+1)*uart_send_len])
            #print("写总字节数:",result)
            #十六进制的读取
            ser.read(15)
            #print(ser.read(15).decode('utf-8'))

            #print("---------------")
            ser.close()#关闭串口

            transfer_progress_refresh()

        except Exception as e:
            print("---异常---：",e)
            break
    
    print("")
    print("音频文件传输完成，按KEY3进入播放模式")
    print("---------------")
    
    '''
    send_cnt[0] = 0
    for file in os.listdir(root):
        print(file)
        audio_transfer(file)

    #TK_Proj.destroy()

    print("")
    print("音频文件传输完成，按中键进入播放模式")
    print("---------------")

    for i in range(len(audio_name_list)):
        print(audio_name_list[i][:-4])
        #print(audio_name_list[i][:-4])
    print("---------------")
    '''
def play_audio_file():
    #num=int(input("请输入需要播放的文件编号："))
    if len(play_num_entry.get()) == 0:
        print('请输入音频文件编号')
    else:
        num=int(play_num_entry.get())
        try:
            portx=UART_PORT
            bps=baudrate
            #超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
            timex=10
            ser=serial.Serial(portx,bps,timeout=timex)
            #print("串口详情参数：", ser)
            result=ser.write(num.to_bytes(1, byteorder="little"))
            #print("播放中")
            print('开始播放文件：',audio_name_list[num])
            #result=ser.write(num.encode('utf-8'))
            #print("写总字节数:",result)
            #十六进制的读取
            ser.read(1)
            print("播放完成")
            print("---------------")
            ser.close()#关闭串口

        except Exception as e:
            print("---异常---：",e)

TK_Proj=tkinter.Tk()
TK_Proj.title('PWM模拟DAC')
TK_Proj['height']=175
TK_Proj['width']=400

# 获取屏幕宽度和高度
screenWidth = TK_Proj.winfo_screenwidth()
screenHeight = TK_Proj.winfo_screenheight()

# 计算窗口显示时的左上角坐标
centerX = int((screenWidth - TK_Proj.winfo_reqwidth()) / 2)
centerY = int((screenHeight - TK_Proj.winfo_reqheight()) / 2)

# 设置窗口显示时的左上角坐标
TK_Proj.geometry("+{}+{}".format(centerX , centerY ))

bg_color_no_active='#8BC34A'
bg_color_active='#5AB963'

info_gen_button=tkinter.Button(TK_Proj,text='生成文件信息',command=info_generate,height=1,width=19,bg=bg_color_no_active,fg='white',
                 activebackground=bg_color_active,activeforeground='white')
info_gen_button.place(x=10,y=10)

transfer_button=tkinter.Button(TK_Proj,text='发送音频文件',command=transfer_all_file,height=1,width=19,bg=bg_color_no_active,fg='white',
                 activebackground=bg_color_active,activeforeground='white')
transfer_button.place(x=207,y=10)

play_button=tkinter.Button(TK_Proj,text='播放音频文件',command=play_audio_file,height=1,width=19,bg=bg_color_no_active,fg='white',
                 activebackground=bg_color_active,activeforeground='white')
play_button.place(x=207,y=60)

text = tkinter.StringVar()
text.set('0/'+str(send_num[0]))
label=tkinter.Label(TK_Proj,textvariable=text,font=('黑体', 12))
label.place(x=175,y=105)

play_num_entry_text=tkinter.Label(TK_Proj,text='音频编号：',font=('黑体', 12))
play_num_entry_text.place(x=10,y=65)

play_num_entry=tkinter.Entry(TK_Proj,width=6,font=('黑体', 12))
play_num_entry.place(x=125,y=65)

progressbar=ttk.Progressbar(TK_Proj)
progressbar.place(x=10,y=140)
progressbar['maximum']=send_num[0]
progressbar['length']=380

TK_Proj.mainloop()
