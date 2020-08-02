import os


import socket
import struct
import threading
import tkinter.filedialog
from tkinter import *

import gnupg
import decrypt


class Server():
    def __init__(self, ip_inaddress, in_port, ip_outaddress, out_port):
        #self.init_window_name=init_window_name
        self.ip_inaddress=ip_inaddress
        self.in_port=in_port
        self.ip_outaddress=ip_outaddress
        self.out_port=out_port
        print(self.ip_inaddress,self.ip_outaddress,self.in_port,self.out_port)

    # 设置窗口?
    #def set_init_window(self)

    def receivefile(self):
        receivesock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 建立TCP连接
        receivesock.bind((self.ip_inaddress, self.in_port))  # 将套接字绑定到地址
        receivesock.listen(3)  # 可建立socket连接的排队的个数，Linux为无限大
        count = 1

        while True:
            print("start.......")

            """调用accept()方法时，socket会进入"waiting"状态。客户请求连接时，方法
            建立连接并返回服务器.accept()方法返回一个含有两个元素的元组。第一个元素
            是新的socket对象，服务器通过该socket对象与客户通信；第二个元素是客户的
            internet地址"""
            sock, adddr = receivesock.accept()

            """recv()方法从客户端接收信息，最后返回一个字符串来表示收到的数据。调用
            一个整数来控制本次调用所接受的最大数据量。"""
            """calcsize()方法计算给定的格式（intenger）占用的字节数"""
            data_size = sock.recv(4)

            # 按格式（intenger）解析接收到的字符串
            total_size = struct.unpack("i", data_size)[0]
            outfile_name = "copy" + str(count)

            outfile = open(outfile_name, "wb")
            if_break = 2  # 没有可读取字符时关闭文件
            data = sock.recv(4)  # 前4字节不读取 # ？不明原因，可能和一开始发送的字符数变量有关 —— Bruce
            while total_size > 0:
                readlen = 1024
                if total_size < readlen:
                    readlen = total_size

                data = sock.recv(readlen)
                outfile.write(data)
                total_size -= len(data)
                if len(data) == 0:  # 检查是否还有字节发送过来
                    if_break -= 1

                if if_break == 0:
                    break  # 没有可读取字符时关闭文件

            outfile.close()
            decryptfilename="decrypted"+str(count)
            decrypt.decryptor(outfile_name, "mykeyfile.asc", "decrypted")  # 解密文件

            count = count + 1
            print(count)

    def sendfile(self):
        gpg = gnupg.GPG()
        keyfile_name = 'mykeyfile.asc'  # path + keyfile
        with open(keyfile_name) as f:
            key_data = f.read()
        import_result = gpg.import_keys(key_data)

        filepath = "copy1"

        try:
            # create an AF_INET, STREAM socket (TCP)
            sendsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except(socket.error, msg):
            print('Failed to create socket. Error code: ' + str(msg[0]) + ' , Error message : ' + msg[1])
            os._exit()

        # client = socket.socket(socket.AF_INET,socket.SOCK_STREAM)#创建TCP连接 AF_INET服务器之间网络通信
        # SOCK_STREAM-TCP
        port_of_target = 3008
        sendsock.connect(("ipaddress_of_target", port_of_target))  # 连接到给定套接字

        print("connect success....")

        stream = open(filepath, 'rb')
        g = gpg.encrypt_file(stream, recipients=['me@email.com'])
        print(str(g))
        fl = bytes(str(g), 'ascii')
        # img = open('encrypted',"rb") #二进制打开文件用于读写

        # size =  os.stat(filepath).st_size #得到文件的大小，以位(bit)为单位
        size = len(fl)
        f = struct.pack("l", size)  # 将C中long类型的数据打包成python中intenger类型
        sendsock.send(f)  # 发送TCP数据，将f中的数据发送到连接的套接字，返回要发送的字节数量

        if g.ok == True:
            client.sendall(fl)  # 完整发送TCP数据，将原文件中读取的数据发送到连接的套接字
        else:
            print('failed to encrypt the file')

        # img.close() #关闭文件
        client.close()  # 关闭套接字