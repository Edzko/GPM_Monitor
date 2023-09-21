# python3.6

import random
import json
from paho.mqtt import client as mqtt_client
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


broker = 'oakmonte.homedns.org'
port = 1883
topic = "Test/DataAnalysis/Vibration/Acquisition"
device = "pm_device11"
# Generate a Client ID with the subscribe prefix.
client_id = f'subscribe-{random.randint(0, 100)}'
# username = 'emqx'
# password = 'public'


def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def subscribe(client: mqtt_client, fig: plt.Figure, line1: plt.Line2D, line2: plt.Line2D):
    def on_message(client, userdata, msg):
        data = json.loads(msg.payload)
        if data['device']==device:
            y = [float(value) for value in data["values"]]
            line1.set_ydata(y)
            y = [float(value) for value in data["valuesOct"]]
            line2.set_ydata(y)
            fig.canvas.draw()
            fig.canvas.flush_events()
            print(f"Received:  RMS=`{data['rms']}` `{msg.payload}` ")

    client.subscribe(topic)
    client.on_message = on_message


def run():
    client = connect_mqtt()
    fig = plt.figure()
    fig.set_label("Frequency data")
    
    x = [30,60,90,120,150,180,210]
    y = [0,0,0,0,0,0,0]
    
    ax1 = fig.add_subplot(211)
    ax1.set_title("Spectrum")
    line1, = ax1.plot(x, y, 'r-')
    ax1.set_ylim(ymin=-0.1, ymax=1)
    ax1.set_xticks(x)
    ax1.grid(True)
    # shift top axis up, to avoid overlap
    pos = ax1.get_position().bounds
    ax1.set_position([pos[0], pos[1]+0.05, pos[2], pos[3]])

    x = [10,20,40,80,160,320,640]
    ax2 = fig.add_subplot(212)
    ax2.set_title("Spectrum (Oct)")
    line2, = ax2.plot(x, y, 'r-')
    ax2.set_ylim(ymin=-0.1, ymax=1)
    ax2.set_xticks(x)
    ax2.grid(True)

    fig.canvas.draw()
    fig.show()
    fig.canvas.flush_events()
    subscribe(client,fig,line1,line2)
    client.loop_forever()


if __name__ == '__main__':
    run()

