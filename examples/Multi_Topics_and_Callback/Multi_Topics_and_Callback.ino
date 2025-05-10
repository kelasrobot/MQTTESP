#include <MQTTESP.h>

// WiFi and MQTT Configuration
const char* WIFI_SSID = "wifi_name";
const char* WIFI_PASSWORD = "wifi_password";
const char* MQTT_SERVER = "broker.emqx.io";
const int MQTT_PORT = 1883;
const char* MQTT_USER = nullptr;
const char* MQTT_PASSWORD = nullptr; 

// MQTT Topics
const char* TOPIC_LIGHT = "home/light";
const char* TOPIC_STATUS = "home/+/status";
const char* TOPIC_SENSOR_ALL = "sensors/#";
const char* TOPIC_TEMPERATURE = "sensors/temperature";
const char* TOPIC_TEMPERATURE_PERSISTENT = "sensors/temperature/persistent";

// Pins
const int LED_PIN = 2;
const int SENSOR_PIN = 34;

// MQTT Client
MQTTESP mqtt(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

// Callback for light control
void handleLightControl(const char* topic, const char* message) {
  Serial.print("Light control message received: ");
  Serial.print(topic);
  Serial.print(" => ");
  Serial.println(message);

  if (strcmp(message, "ON") == 0) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED turned ON");
  } else if (strcmp(message, "OFF") == 0) {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED turned OFF");
  }
}

// Callback for status messages
void handleStatusMessages(const char* topic, const char* message) {
  Serial.print("Status message received: ");
  Serial.print(topic);
  Serial.print(" => ");
  Serial.println(message);
}

// Callback for sensor data
void handleSensorData(const char* topic, const char* message) {
  Serial.print("Sensor data received: ");
  Serial.print(topic);
  Serial.print(" => ");
  Serial.println(message);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  mqtt.begin();

  mqtt.registerCallback(TOPIC_LIGHT, handleLightControl);
  mqtt.registerCallback(TOPIC_STATUS, handleStatusMessages);
  mqtt.registerCallback(TOPIC_SENSOR_ALL, handleSensorData);

  Serial.println("MQTT client started");
}

void loop() {
  mqtt.loop();

  static unsigned long lastSensorUpdate = 0;
  if (millis() - lastSensorUpdate > 5000) {
    int sensorValue = analogRead(SENSOR_PIN);
    float temperature = sensorValue * 0.1;
    String temperatureStr = String(temperature, 1);

    mqtt.publish(TOPIC_TEMPERATURE, temperatureStr);
    mqtt.publish(TOPIC_TEMPERATURE_PERSISTENT, temperatureStr, true, 1);

    Serial.print("Published temperature: ");
    Serial.println(temperatureStr);

    lastSensorUpdate = millis();
  }

  delay(10);
}
