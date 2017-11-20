from random import randint
import thread
import sys
try:
    import paho.mqtt.client as mqtt
except ImportError:
    import os
    import inspect
    cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"../src")))
    if cmd_subfolder not in sys.path:
        sys.path.insert(0, cmd_subfolder)
    import paho.mqtt.client as mqtt


server = "192.168.1.107";
topic = "smarthome/room1/ota"


def on_connect(mqttc, obj, flags, rc):
    print("rc: "+str(rc))

def on_message(mqttc, obj, msg):
    print(msg.topic+" "+str(msg.qos)+" "+str(msg.payload))

def on_publish(mqttc, obj, mid):
    print("mid: "+str(mid))

def on_subscribe(mqttc, obj, mid, granted_qos):
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_log(mqttc, obj, level, string):
    print(string)

mqttc = mqtt.Client()
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe
mqttc.connect(server, 1883, 60)
mqttc.subscribe(topic, 0)


def fotaControl( threadName, delay):
    while True:
        val = raw_input('Enter 1 to update firmware OTA ')
        if(val == "1"):
            mqttc.publish(topic, "url:http://192.168.1.107/upload/led.bin,md5:6bd07139c21f572370242905c4465056")

try:
    thread.start_new_thread( fotaControl, ("Fota Control", 0, ) )
except:
    print "Error: unable to start thread"

mqttc.loop_forever()
