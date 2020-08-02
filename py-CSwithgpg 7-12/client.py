#!/usr/bin/python3
import socket
import struct
import os
import json
import gnupg
import io

gpg = gnupg.GPG()
keyfile_name = 'mykeyfile.asc' # path + keyfile
with open(keyfile_name) as f:
    key_data = f.read()
import_result = gpg.import_keys(key_data)

filepath="A" #input("please input the path of the file: ") #得到原文件的路径

try:
    #create an AF_INET, STREAM socket (TCP)
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except(socket.error, msg):
    print('Failed to create socket. Error code: ' + str(msg[0]) + ' , Error message : ' + msg[1])
    os._exit()

#client = socket.socket(socket.AF_INET,socket.SOCK_STREAM)#创建TCP连接 AF_INET服务器之间网络通信
#SOCK_STREAM-TCP
client.connect(("127.0.0.1",3008))#连接到给定套接字

print("connect success....")

stream = open(filepath, 'rb')
g = gpg.encrypt_file(stream, recipients=['me@email.com'])
print(str(g))
fl = bytes(str(g), 'ascii')
#img = open('encrypted',"rb") #二进制打开文件用于读写

#size =  os.stat(filepath).st_size #得到文件的大小，以位(bit)为单位
size=len(fl)
f= struct.pack("l",size) #将C中long类型的数据打包成python中intenger类型
client.send(f) #发送TCP数据，将f中的数据发送到连接的套接字，返回要发送的字节数量


if g.ok==True:
    client.sendall(fl) #完整发送TCP数据，将原文件中读取的数据发送到连接的套接字
else:
    print('failed to encrypt the file')

#img.close() #关闭文件
client.close() #关闭套接字

