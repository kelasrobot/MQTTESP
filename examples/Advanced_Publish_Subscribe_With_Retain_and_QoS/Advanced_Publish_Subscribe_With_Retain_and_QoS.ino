#include <MQTTESP.h>
const char* ssid = "isi dengan nama wifi";
const char* password = "isi dengan password wifi";
const char* mqtt_server = "broker.emqx.io";
MQTTESP mqtt(ssid, password, mqtt_server);

const char* topicSensor = "tes/topic/sensor";
const char* topicLampu = "tes/topic/lampu";

unsigned const long intervalKirim = 1000;
unsigned long timer, counter = 0;

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  mqtt.begin();
  mqtt.subscribe(topicLampu, 1); //QoS 1
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= intervalKirim) {
    timer = millis();
    counter++;
    mqtt.publish(topicSensor, String(counter), true, 2); //Retain & QoS 2
  }

  String pesan = mqtt.getIncomingMessage();
  if (pesan != "") {
    digitalWrite(2, pesan.toInt());
    mqtt.setIncomingMessage("");
  }

}
