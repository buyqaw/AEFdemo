#!/usr/bin/env python3
# -*- coding: utf-8 -*-

__author__ = "Bauyrzhan Ospan"
__copyright__ = "Copyright 2018, Cleverest Technologies"
__version__ = "1.0.1"
__maintainer__ = "Bauyrzhan Ospan"
__email__ = "bospan@cleverest.tech"
__status__ = "Development"

from gevent import monkey
monkey.patch_all()

from flask import Flask, render_template, session, request, json, jsonify, url_for, Markup, redirect
from flask_socketio import SocketIO, emit, join_room, leave_room, \
	close_room, rooms, disconnect
from threading import Lock

async_mode = None

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app, async_mode=async_mode)
thread = None
thread_lock = Lock()


@app.route('/show/', methods=['GET', 'POST'])
def show():
    return render_template('index.html')


@app.route('/BLE/', methods=['GET', 'POST'])
def BLE():
    return render_template('get_char.html')


@app.route('/index/<name>', methods=['GET', 'POST'])
def index(name):
    # text = request.form['FIOin'] + ":" + request.form['FIOout'] + ":" + request.form['room']
    print(name)
    socketio.emit('my response', name, namespace='/names')
    # return render_template('index.html')
    return "ok"


@socketio.on('connect', namespace='/names')
def test_connect():
    print("Socket is started")


if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=3737, debug=True)
