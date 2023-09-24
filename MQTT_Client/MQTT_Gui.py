import random
import json
from paho.mqtt import client as mqtt_client
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import dearpygui.dearpygui as dpg

broker = 'oakmonte.homedns.org'
port = 1883
device = "pm_device11"
# Generate a Client ID with the subscribe prefix.
client_id = f'subscribe-{random.randint(0, 100)}'
# username = 'emqx'
# password = 'public'

topics = []
devices = []
rms = 0.0

def save_callback():
    print("Save Clicked")
    
def select_Topic(sender, value):
    print(value)
def select_Device(sender, value):
    print(value)

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
        try:
            data = json.loads(msg.payload)
            found = False
            for topic in topics:
                if topic == msg.topic:
                    found = True
            if not found:
                topics.append(msg.topic)
                dpg.configure_item(topiclist,items=topics)    
                print(f"Topic:  `{msg.topic}` ")
            for device in devices:
                if device == data['device']:
                    found = True
            if not found:
                devices.append(data['device'])
                dpg.configure_item(devicelist,items=devices)    
                print(f"Device:  `{data['device']}` ")
            rms = data['rms']
            dpg.set_value("rms",rms)
            dpg.set_value("version",f"Firmware: `{data['firmware']}`")
        except:
            print(f"Bad message: `{msg.payload}` ")
    # Subscribe to all topics on the broker
    client.subscribe("#")
    client.on_message = on_message

dpg.create_context()
dpg.create_viewport(title='Vibration Analytics', width=600, height=200)
dpg.setup_dearpygui()

# Create the GUI window
with dpg.font_registry():
    # Download font here: https://fonts.google.com/specimen/Open+Sans
    default_font = dpg.add_font("OpenSans-Regular.ttf", 15, tag="ttf-font")
    
with dpg.window(label="Management Window", no_resize=True) as main_window:
    dpg.add_text("Hello world")
    dpg.add_button(label="Save", callback=save_callback)
    dpg.add_input_text(label="string", width=200)
    dpg.add_slider_float(label="float",width=200)
    topiclist = dpg.add_combo(label="Topics", items=topics, callback=select_Topic,width=200)
    devicelist = dpg.add_combo(label="Devices", items=devices, callback=select_Device,width=200)
    dpg.bind_font(default_font)
    dpg.add_text("Device",pos=(300,10))
    dpg.add_text("Version",pos=(300,30),tag="version")
    dpg.add_input_text(label="RMS",pos=(300,50),tag="rms", width=200)

dpg.set_primary_window(main_window, True) # Should fill viewport but does not on macOS
dpg.show_viewport()

client = connect_mqtt()
subscribe(client)
    
while dpg.is_dearpygui_running():
    # insert here any code you would like to run in the render loop
    # you can manually stop by using stop_dearpygui()
    # print("this will run every frame")
    client.loop(timeout = 0.1, max_packets=1)
    dpg.render_dearpygui_frame()
    
dpg.destroy_context()









def run():
    
    client.loop_forever()