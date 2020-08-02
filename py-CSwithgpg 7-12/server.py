import socket
import struct
import decrypt

"""开启接收端，等待发送端请求连接"""
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 建立TCP连接
server.bind(("0.0.0.0", 3008))  # 将套接字绑定到地址
server.listen(3)  # 可建立socket连接的排队的个数，Linux为无限大
count = 1
while True:
    print("start.......")

    """调用accept()方法时，socket会进入"waiting"状态。客户请求连接时，方法
    建立连接并返回服务器.accept()方法返回一个含有两个元素的元组。第一个元素
    是新的socket对象，服务器通过该socket对象与客户通信；第二个元素是客户的
    internet地址"""
    sock, adddr = server.accept()

    """recv()方法从客户端接收信息，最后返回一个字符串来表示收到的数据。调用
    一个整数来控制本次调用所接受的最大数据量。"""
    """calcsize()方法计算给定的格式（intenger）占用的字节数"""
    data_size = sock.recv(4)

    # 按格式（intenger）解析接收到的字符串
    total_size = struct.unpack("i", data_size)[0]
    outfile_name = "copy" + str(count)
    
    outfile=open(outfile_name,"wb")
    if_break = 2 # 没有可读取字符时关闭文件
    data = sock.recv(4) # 前4字节不读取 # ？不明原因，可能和一开始发送的字符数变量有关 —— Bruce
    while total_size>0:
        readlen=1024
        if total_size<readlen:
            readlen=total_size

        data=sock.recv(readlen)
        outfile.write(data)            
        total_size-=len(data)
        if len(data) == 0: # 检查是否还有字节发送过来
            if_break -= 1
            
        if if_break == 0:
            break # 没有可读取字符时关闭文件
            
    outfile.close()

    #解密
    decrypt.decryptor(outfile_name, "mykeyfile.asc", "decrypted") # 解密文件
    count = count + 1
    print(count)
