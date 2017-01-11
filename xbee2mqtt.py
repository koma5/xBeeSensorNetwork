from xbee import XBee, ZigBee
import paho.mqtt.client as paho
import serial

topicMap = {'1': "xbee/volt",
            '2': "xbee/temp",
            '3': "xbee/hum"}

ser = serial.Serial('/dev/cu.usbserial-A402LDNO', 9600)

xbee = ZigBee(ser,escaped=True)

def on_connect(client, userdata, flags, rc):
    print("CONNACK received with code %d." % (rc))

client = paho.Client()
client.on_connect = on_connect
client.connect("172.16.0.70", 1883)
client.loop_start()


# Continuously read and print packets
while True:
    try:
        response = xbee.wait_read_frame()
        data = response['rf_data']
        topicId, message =  data.decode().split(':')
        (rc, mid) = client.publish(topicMap[topicId], message, qos=0)
        print(data)
    except KeyboardInterrupt:
        break

ser.close()
