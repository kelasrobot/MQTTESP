#include "MQTTESP.h"

MQTTESP::MQTTESP(const char* ssid, const char* password, const char* mqtt_server,
                 int mqtt_port, const char* mqtt_user, const char* mqtt_pass)
  : _ssid(ssid), _password(password), _mqtt_server(mqtt_server), _mqtt_port(mqtt_port),
    _mqtt_user(mqtt_user), _mqtt_pass(mqtt_pass), _client(1024) {
  memset(_incomingTopic, 0, sizeof(_incomingTopic));
  memset(_incomingMessage, 0, sizeof(_incomingMessage));
}

void MQTTESP::begin() {
  connectWiFi();
  _client.begin(_mqtt_server, _mqtt_port, _net);
  _client.onMessage([this](String &topic, String &payload) {
    this->onMessage(topic, payload);
  });
  connectMQTT();
}

void MQTTESP::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  _client.loop();
  if (!_client.connected()) {
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
  String clientId = "mqttesp_client_id-" + String(random(0xffff), HEX) + String(random(0xffff), HEX) + String(random(0xffff), HEX);
  Serial.println("[MQTT]: Client ID: " + clientId);
  Serial.println("[MQTT]: MQTT Server: " + String(_mqtt_server));
  Serial.println("[MQTT]: MQTT Port: " + String(_mqtt_port));
  Serial.println("[MQTT]: MQTT User: " + String(_mqtt_user ? _mqtt_user : "None"));
  Serial.println("[MQTT]: MQTT Pass: " + String(_mqtt_pass ? _mqtt_pass : "None"));

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

  for (int i = 0; i < _callbackCount; i++) {
    _client.subscribe(_callbacks[i].topicPattern.c_str());
    Serial.println("[MQTT]: Resubscribed to: " + _callbacks[i].topicPattern);
  }
}

void MQTTESP::onMessageStatic(String &topic, String &payload) {
  // Do nothing, as we use the lambda function in begin()
}

void MQTTESP::registerCallback(const char* topic, MQTTMessageCallback callback) {
  if (_callbackCount >= MAX_CALLBACKS) {
    Serial.println("[MQTT] Error: Max callbacks reached");
    return;
  }
  
  _callbacks[_callbackCount] = {String(topic), callback};
  _callbackCount++;
  
  subscribe(topic);
}

bool MQTTESP::topicMatches(const char* pattern, const char* topic) {
  const char* p = pattern;
  const char* t = topic;
  const char* p_wild = nullptr;
  const char* t_wild = nullptr;

  while (*t) {
    if (*p == '#') {
      return true;
    }
    
    if (*p == '+') {
      p_wild = p++;
      t_wild = t;
      while (*t != '/' && *t) t++;
    } else if (*p == *t) {
      p++;
      t++;
    } else if (p_wild) {
      p = p_wild + 1;
      t = ++t_wild;
    } else {
      return false;
    }
  }

  while (*p == '/' || *p == '+' || *p == '#') p++;
  return *p == '\0' && *t == '\0';
}

void MQTTESP::onMessage(String &topic, String &payload) {
  strlcpy(_incomingTopic, topic.c_str(), sizeof(_incomingTopic));
  strlcpy(_incomingMessage, payload.c_str(), sizeof(_incomingMessage));

  Serial.printf("[MQTT] Received: %s => %s\n", _incomingTopic, _incomingMessage);

  for (int i = 0; i < _callbackCount; i++) {
    if (topicMatches(_callbacks[i].topicPattern.c_str(), _incomingTopic)) {
      _callbacks[i].callback(_incomingTopic, _incomingMessage);
    }
  }
}
