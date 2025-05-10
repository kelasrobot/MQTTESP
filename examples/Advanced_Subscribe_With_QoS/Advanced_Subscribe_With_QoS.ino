#include <MQTTESP.h>
const char* ssid = "wifi_name";
const char* password = "wifi_password";
const char* mqtt_server = "broker.emqx.io";
MQTTESP mqtt(ssid, password, mqtt_server);
const char* topicLampu = "tes/topic/lampu";

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  mqtt.begin();
  mqtt.subscribe(topicLampu, 1); //QoS 1
}

void loop() {
  mqtt.loop();
  String pesan = mqtt.getIncomingMessage();
  if (pesan != "") {
    digitalWrite(2, pesan.toInt());
    mqtt.setIncomingMessage("");
  }
}
