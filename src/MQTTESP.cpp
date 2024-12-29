#include "MQTTESP.h"

MQTTESP::MQTTESP(const char* ssid, const char* password, const char* mqtt_server,
                 int mqtt_port, const char* mqtt_user, const char* mqtt_pass)
  : _ssid(ssid), _password(password), _mqtt_server(mqtt_server), _mqtt_port(mqtt_port),
    _mqtt_user(mqtt_user), _mqtt_pass(mqtt_pass) {
}

void MQTTESP::begin() {
  connectWiFi();
  _client.begin(_mqtt_server, _mqtt_port, _net);
  _client.onMessage([this](String & topic, String & payload) {
    this->onMessage(topic, payload);
  });
  connectMQTT();
}

void MQTTESP::loop() {
  _client.loop();
  if (!isConnected()) {
    connectMQTT();
  }
}

bool MQTTESP::isConnected() {
  return _client.connected();
}

void MQTTESP::publish(String topic, String payload) {
  bool result = _client.publish(topic, payload);
  if (result) {
    Serial.println("[MQTT]: Publish successful");
    Serial.println("[MQTT]: Topic: " + topic);
    Serial.println("[MQTT]: Payload: " + payload);
    Serial.println();
  } else {
    Serial.println("[MQTT]: Publish failed");
  }
}

void MQTTESP::publish(String topic, String payload, bool retained, int qos) {
  bool result = _client.publish(topic, payload, retained, qos);
  if (result) {
    Serial.println("[MQTT]: Publish successful");
    Serial.println("[MQTT]: Topic: " + topic);
    Serial.println("[MQTT]: Payload: " + payload);
    Serial.println("[MQTT]: Retained: " + String(retained));
    Serial.println("[MQTT]: QoS: " + String(qos));
    Serial.println();
  } else {
    Serial.println("[MQTT]: Publish failed");
  }
}

void MQTTESP::subscribe(const char* topic) {
  bool result = _client.subscribe(topic);
  if (result) {
    Serial.println("[MQTT]: Subscribe successful");
    Serial.println("[MQTT]: Topic: " + String(topic));
    Serial.println();
  } else {
    Serial.println("[MQTT]: Subscribe failed");
  }
}

void MQTTESP::subscribe(const char* topic, int qos) {
  bool result = _client.subscribe(topic, qos);
  if (result) {
    Serial.println("[MQTT]: Subscribe successful");
    Serial.println("[MQTT]: Topic: " + String(topic));
    Serial.println("[MQTT]: QoS: " + String(qos));
    Serial.println();
  } else {
    Serial.println("[MQTT]: Subscribe failed");
  }
}

const char* MQTTESP::getIncomingTopic() {
  return _incomingTopic;
}

const char* MQTTESP::getIncomingMessage() {
  return _incomingMessage;
}

void MQTTESP::setIncomingTopic(const char* topic) {
  strncpy(_incomingTopic, topic, sizeof(_incomingTopic) - 1);
  _incomingTopic[sizeof(_incomingTopic) - 1] = '\0';
}

void MQTTESP::setIncomingMessage(const char* message) {
  strncpy(_incomingMessage, message, sizeof(_incomingMessage) - 1);
  _incomingMessage[sizeof(_incomingMessage) - 1] = '\0';
}

void MQTTESP::connectWiFi() {
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("[WIFI]: Connecting to WiFi...");
  }
  Serial.println("[WIFI]: WiFi connected");
}

void MQTTESP::connectMQTT() {
  String clientId = "mqttesp_client_id-" + String(random(0xffff), HEX);
  if (_mqtt_user && _mqtt_pass) {
    while (!_client.connect(clientId.c_str(), _mqtt_user, _mqtt_pass)) {
      Serial.println("[MQTT]: Connecting to MQTT...");
      delay(1000);
    }
  } else {
    while (!_client.connect(clientId.c_str())) {
      Serial.println("[MQTT]: Connecting to MQTT...");
      delay(1000);
    }
  }
  Serial.println("[MQTT]: MQTT connected");
}

void MQTTESP::setWill(const char* topic, const char* payload, bool retained, int qos) {
  _client.setWill(topic, payload, retained, qos);
  Serial.println("[MQTT]: Last Will set");
  Serial.println("[MQTT]: Topic: " + String(topic));
  Serial.println("[MQTT]: Payload: " + String(payload));
  Serial.println("[MQTT]: Retained: " + String(retained));
  Serial.println("[MQTT]: QoS: " + String(qos));
}

void MQTTESP::clearWill() {
  _client.clearWill();
  Serial.println("[MQTT]: Last Will cleared");
}

void MQTTESP::onMessage(String &topic, String &payload) {
  setIncomingTopic(topic.c_str());
  setIncomingMessage(payload.c_str());
  Serial.print("[MQTT] Incoming Topic: ");
  Serial.println(_incomingTopic);
  Serial.print("[MQTT] Incoming Message: ");
  Serial.println(_incomingMessage);
  Serial.println();
}