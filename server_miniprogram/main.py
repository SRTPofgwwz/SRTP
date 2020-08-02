import socket
import threading
import tkinter.filedialog
from tkinter import *

import ServerC

def get_start():
    ip_inaddress='0.0.0.0'
    in_port=8888
    ip_outaddress='127.0.0.1'
    out_port=8888

    server=ServerC.Server(ip_inaddress,in_port,ip_outaddress,out_port)

    thread_receive=threading.Thread(target=server.receivefile())
    thread_receive.start()



if __name__=="__main__":
    get_start()