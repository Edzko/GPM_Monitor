import random
import json
from paho.mqtt import client as mqtt_client
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


broker = 'oakmonte.homedns.org'
port = 1883
#topic = "Test/DataAnalysis/Vibration/Acquisition"
topic = "#"
device = "pm_device11"
# Generate a Client ID with the subscribe prefix.
client_id = f'subscribe-{random.randint(0, 100)}'
# username = 'emqx'
# password = 'public'

topics = []


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


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        #data = json.loads(msg.payload)
        #print(f"Received:  RMS=`{data['rms']}` `{msg.payload}` ")
        found = False
        for topic in topics:
            if topic == msg.topic:
                found = True
        if not found:
            topics.append(msg.topic)
            print(f"Topic:  `{msg.topic}` ")

    client.subscribe(topic)
    client.on_message = on_message


def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()


if __name__ == '__main__':
    run()
