#include <XBee.h>
#include <dht.h>
#include <Vcc.h>

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
void sendStringFloat(char topic, float message) {
  if(message <= -10) { //5 chars  -10.0 / -99.9
    dtostrf(message, 3, 1, sensorStringBuf); //min length bigger is no problem
    uint8_t payload[7];
    payload[0] = topic; payload[1] = ':';
    for(int i = 2; i < 7; i++) {
      payload[i] = sensorStringBuf[i-2];
    }
    ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
    xbee.send(zbTx);
  }
  else if (message < 0 && message > -10) { //4 chars  -9.9 / -1.1
    dtostrf(message, 3, 1, sensorStringBuf); //min length bigger is no problem
    uint8_t payload[6];
    payload[0] = topic; payload[1] = ':';
    for(int i = 2; i < 6; i++) {
      payload[i] = sensorStringBuf[i-2];
    }
    ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
    xbee.send(zbTx);
  }
  else if (message >= 0 &&  message < 10) { //3 chars  9.9 / 0.0
    dtostrf(message, 3, 1, sensorStringBuf); //min length bigger is no problem
    uint8_t payload[5];
    payload[0] = topic; payload[1] = ':';
    for(int i = 2; i < 5; i++) {
      payload[i] = sensorStringBuf[i-2];
    }
    ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
    xbee.send(zbTx);
  }
  else { //>= 10 4 chars  9.9 / 0.0
    dtostrf(message, 3, 1, sensorStringBuf); //min length bigger is no problem
    uint8_t payload[6];
    payload[0] = topic; payload[1] = ':';
    for(int i = 2; i < 6; i++) {
      payload[i] = sensorStringBuf[i-2];
    }
    ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
    xbee.send(zbTx);
  }
}


const float VccCorrection = 5.06/5.07;  // Measured Vcc by multimeter divided by reported Vcc
unsigned long lastVoltCheck = 0;
unsigned long lastVoltPub = 0;
float lastV = 0;

Vcc vcc(VccCorrection);

void loop() {
  if(millis() < lastVoltCheck || millis() - lastVoltCheck >= 2000) {
    float v = vcc.Read_Volts();
    if(v != lastV || millis() - lastVoltPub >= 10000) {

      dtostrf(v, 4, 2, sensorStringBuf);
      uint8_t payload[6];
      payload[0] = '1' ; payload[1] = ':';
      for(int i = 2; i < 6; i++) {
        payload[i] = sensorStringBuf[i-2];
      }
      ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
      xbee.send(zbTx);
      lastVoltPub = millis();
    }
    lastV = v;
    lastVoltCheck = millis();
  }

  for(int sensor=0; sensor<SENSOR_COUNT; sensor++) {
    if(millis() < sensors[sensor].lastCheck || millis() - sensors[sensor].lastCheck >= 2000)
    {
      int chk = DHT.read22(sensors[sensor].DHT22_PIN);
    
      sensors[sensor].hum = DHT.humidity;
      sensors[sensor].temp  = DHT.temperature;
    
      if (chk == DHTLIB_OK && (sensors[sensor].temp != sensors[sensor].lastTemp || millis() - sensors[sensor].lastTempPub >= 10000))
      {
        sendStringFloat('2', sensors[sensor].temp);

        sensors[sensor].lastTempPub = millis();
      }
      if (chk == DHTLIB_OK && (sensors[sensor].hum != sensors[sensor].lastHum || millis() - sensors[sensor].lastHumPub >= 10000))
      {
        sendStringFloat('3', sensors[sensor].hum);

        sensors[sensor].lastHumPub = millis();
      }
      sensors[sensor].lastHum = sensors[sensor].hum;
      sensors[sensor].lastTemp = sensors[sensor].temp;
      sensors[sensor].lastCheck = millis();
     }
  }
}
