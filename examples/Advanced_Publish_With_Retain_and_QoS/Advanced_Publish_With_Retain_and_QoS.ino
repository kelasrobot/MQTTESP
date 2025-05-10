#include <MQTTESP.h>
const char* ssid = "wifi_name";
const char* password = "wifi_password";
const char* mqtt_server = "broker.emqx.io";
MQTTESP mqtt(ssid, password, mqtt_server);

const char* topicSensor = "tes/topic/sensor";

unsigned const long intervalKirim = 1000;
unsigned long timer, counter = 0;

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  mqtt.begin();
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= intervalKirim) {
    timer = millis();
    counter++;
    mqtt.publish(topicSensor, String(counter), true, 2); //Retain True, QoS 2
  }

}
