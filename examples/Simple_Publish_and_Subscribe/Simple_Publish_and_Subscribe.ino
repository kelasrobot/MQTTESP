#include <MQTTESP.h>
const char* ssid = "isi nama wifi";
const char* password = "isi password wifi";
const char* mqtt_server = "broker.emqx.io";
MQTTESP mqtt(ssid, password, mqtt_server);

const char* topicSensor = "tes/topic/sensor";
const char* topicLampu = "tes/topic/lampu";

unsigned long timer, counter = 0, intervalKirim = 1000;

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  mqtt.begin();
  mqtt.subscribe(topicLampu);
}

void loop() {
  mqtt.loop();
  
  if (millis() - timer >= intervalKirim) {
    timer = millis();
    mqtt.publish(topicSensor, String(++counter));
  }
  
  String pesan = mqtt.getIncomingMessage();
  if (pesan != "") {
    digitalWrite(2, pesan.toInt());
    mqtt.setIncomingMessage("");
  }
  
}