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


def subscribe(client: mqtt_client, fig: plt.Figure, ax1: plt.Axes, line1: plt.Line2D):
    def on_message(client, userdata, msg):
        data = json.loads(msg.payload)
        if data['device']==device:
            fmax = data["fmax"]
            y = [float(value) for value in data["values"]]
            x = [value * fmax / 128 for value in list(np.arange(1,128+1))]
            line1.set_ydata(y)
            line1.set_xdata(x)
            if fmax<200:
                ax1.set_xticks(list(np.arange(0,fmax,10)))
            elif fmax<500:
                ax1.set_xticks(list(np.arange(0,fmax,25)))
            else:
                ax1.set_xticks(list(np.arange(0,fmax,50)))
            fig.canvas.draw()
            fig.canvas.flush_events()
            print(f"Received:  RMS=`{data['rms']}` `{msg.payload}` ")

    client.subscribe(topic)
    client.on_message = on_message


def run():
    client = connect_mqtt()
    fig = plt.figure()
    fig.set_label("Frequency data")
    
    x = list(np.arange(1,128+1))
    y = [0 for element in range(128)]
    
    ax1 = fig.add_subplot(111)
    ax1.set_title("Spectrum")
    line1, = ax1.plot(x, y, 'r-')
    ax1.set_ylim(ymin=0, ymax=150)
    ax1.set_xlabel("Freq [Hz]")
    #ax1.set_xticks([0, 32, 64, 96, 128])
    ax1.grid(True)

    fig.canvas.draw()
    fig.show()
    fig.canvas.flush_events()
    subscribe(client,fig,ax1,line1)
    client.loop_forever()


if __name__ == '__main__':
    run()

