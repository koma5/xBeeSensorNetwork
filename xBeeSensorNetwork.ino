#include <XBee.h>
#include <dht.h>

dht DHT;

XBee xbee = XBee();

typedef struct {

  int DHT22_PIN;

  float hum;
  float temp;

  float lastHum;
  float lastTemp;

  unsigned long lastCheck;

  unsigned long lastTempPub;
  unsigned long lastHumPub;

} sensor;

#define SENSOR_COUNT 1

sensor sensors[SENSOR_COUNT] = {
  {
    8,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  }
};

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x13A200, 0x40BA9142);

char sensorStringBuf[25];

void setup() {
Serial.begin(9600);
xbee.setSerial(Serial);

}


void loop() {
  for(int sensor=0; sensor<SENSOR_COUNT; sensor++) {
    if(millis() < sensors[sensor].lastCheck || millis() - sensors[sensor].lastCheck >= 2000)
    {
      int chk = DHT.read22(sensors[sensor].DHT22_PIN);
    
      sensors[sensor].hum = DHT.humidity;
      sensors[sensor].temp  = DHT.temperature;
    
      if (chk == DHTLIB_OK && (sensors[sensor].temp != sensors[sensor].lastTemp || millis() - sensors[sensor].lastTempPub >= 10000))
      {
    
        if (sensors[sensor].temp >= 10 || sensors[sensor].temp <= -10) {
          dtostrf(sensors[sensor].temp, 4, 1, sensorStringBuf);
          uint8_t payload[5];
          for(int i = 0; i < 5; i++) {
            payload[i] = sensorStringBuf[i];
          }
          ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
          xbee.send(zbTx);
        }
        else {
          dtostrf(sensors[sensor].temp, 4, 1, sensorStringBuf);
          uint8_t payload[4];
          for(int i = 0; i < 4; i++) {
            payload[i] = sensorStringBuf[i];
          }
          ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
          xbee.send(zbTx);
        }

        sensors[sensor].lastTempPub = millis();
      }
      if (chk == DHTLIB_OK && (sensors[sensor].hum != sensors[sensor].lastHum || millis() - sensors[sensor].lastHumPub >= 10000))
      {
        if (sensors[sensor].hum >= 10) {
          dtostrf(sensors[sensor].hum, 4, 1, sensorStringBuf);
          uint8_t payload[5];
          for(int i = 0; i < 5; i++) {
            payload[i] = sensorStringBuf[i];
          }
          ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
          xbee.send(zbTx);
        }
        else {
          dtostrf(sensors[sensor].hum, 4, 1, sensorStringBuf);
          uint8_t payload[4];
          for(int i = 0; i < 4; i++) {
            payload[i] = sensorStringBuf[i];
          }
          ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
          xbee.send(zbTx);
        }

        sensors[sensor].lastHumPub = millis();
      }
      sensors[sensor].lastHum = sensors[sensor].hum;
      sensors[sensor].lastTemp = sensors[sensor].temp;
      sensors[sensor].lastCheck = millis();
     }
  }
}
