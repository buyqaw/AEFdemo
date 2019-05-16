#!/usr/bin/env python3
# -*- coding: utf-8 -*-

__author__ = "Bauyrzhan Ospan"
__copyright__ = "Copyright 2019, Buyqaw LLP"
__version__ = "1.0.1"
__maintainer__ = "Bauyrzhan Ospan"
__email__ = "bospan@cleverest.tech"
__status__ = "Development"

# standard import of os
import os

# import socket programming library
import socket
import time
import requests

# import python-mongo library
from pymongo import MongoClient

# import thread module
from _thread import *
import threading

# import module to parse json
import json

# import datetime to deal with timestamps
from datetime import datetime

import telepot


# global variables
print_lock = threading.Lock()
client = MongoClient('mongodb://database:27017/')
db = client.buyqaw


# functions
def lograw(addr, data):
    print("From: " + str(addr))
    item_doc = {
        'data': data,
        'IP': addr[0],
        'port': addr[1],
        'date': datetime.now()
    }
    db.rawlog.insert_one(item_doc)

# Telegram message
def send_tlg_msg(msg):
    ids = ["https://t.me/buyqawaef2019"]
    bot = telepot.Bot('839455204:AAEURORDcMya-awnUX2NXhe2DFdVAuOOaCc')
    for id in ids:
        try:
           print(bot.sendMessage(str(id), str(msg)))
        except:
           print("No connection to telegram")
           pass


# thread function
def threaded(c, addr):
    try:
        while True:

            # data received from client
            data = c.recv(50000).decode('utf-8')
            if not data:
                print('Bye')

                # lock released on exit
                print_lock.release()
                break
            print(data)
            data = str(data).split("=")[0]
            print(data)
            print_lock.release()
            c.close()
            send_tlg_msg(data)
            lograw(addr, data)
            break


            # connection closed

    except Exception as ex:
        c.send(str("ERROR [505]: execution leads to internal error:" + str(ex)).encode('utf-8'))
        c.close()


def Main():
    host = "0.0.0.0"

    # reverse a port on your computer
    # in our case it is 12345 but it
    # can be anything
    port = 3737
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host, port))
    print("socket binded to post", port)

    # put the socket into listening mode
    s.listen(50)
    print("socket is listening")

    # a forever loop until client wants to exit
    while True:
        # establish connection with client
        c, addr = s.accept()

        # lock acquired by client
        print_lock.acquire()
        print('Connected to :', addr[0], ':', addr[1])

        # Start a new thread and return its identifier
        start_new_thread(threaded, (c, addr,))
    s.close()


if __name__ == '__main__':
    Main()
