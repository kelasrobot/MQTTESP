#include "MQTTESP.h"

MQTTESP::MQTTESP(const char* ssid, const char* password, const char* mqtt_server,
                 int mqtt_port, const char* mqtt_user, const char* mqtt_pass)
  : _ssid(ssid), _password(password), _mqtt_server(mqtt_server), _mqtt_port(mqtt_port),
    _mqtt_user(mqtt_user), _mqtt_pass(mqtt_pass), _client(1024) {
  memset(_incomingTopic, 0, sizeof(_incomingTopic));
  memset(_incomingMessage, 0, sizeof(_incomingMessage));
  _debug = true; // Debug aktif secara default
}

void MQTTESP::begin() {
  debugPrint("MQTTESP Starting...");
  debugPrint(""); // Spasi
  connectWiFi();
  _client.begin(_mqtt_server, _mqtt_port, _net);
  _client.onMessage([this](String &topic, String &payload) {
    this->onMessage(topic, payload);
  });
  connectMQTT();
}

void MQTTESP::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    debugPrint(""); // Spasi
    debugPrint("WiFi disconnected, reconnecting...");
    connectWiFi();
  }
  _client.loop();
  if (!_client.connected()) {
    debugPrint(""); // Spasi
    debugPrint("MQTT disconnected, reconnecting...");
    connectMQTT();
  }
}

bool MQTTESP::isConnected() {
  return _client.connected();
}

void MQTTESP::publish(String topic, String payload) {
  bool result = _client.publish(topic, payload);
  if (_debug) {
    if (result) {
      debugPrint("PUBLISH SUCCESS");
      debugPrint("  Topic: " + topic);
      debugPrint("  Payload: " + payload);
    } else {
      debugPrint("PUBLISH FAILED");
      debugPrint("  Topic: " + topic);
      debugPrint("  Payload: " + payload);
    }
    debugPrint(""); // Spasi setelah publish
  }
}

void MQTTESP::publish(String topic, String payload, bool retained, int qos) {
  bool result = _client.publish(topic, payload, retained, qos);
  if (_debug) {
    if (result) {
      debugPrint("PUBLISH SUCCESS");
      debugPrint("  Topic: " + topic);
      debugPrint("  Payload: " + payload);
      debugPrint("  Retained: " + String(retained ? "true" : "false"));
      debugPrint("  QoS: " + String(qos));
    } else {
      debugPrint("PUBLISH FAILED");
      debugPrint("  Topic: " + topic);
      debugPrint("  Payload: " + payload);
    }
    debugPrint(""); // Spasi setelah publish
  }
}

void MQTTESP::subscribe(const char* topic) {
  subscribe(topic, 0);
}

void MQTTESP::subscribe(const char* topic, int qos) {
  bool result = _client.subscribe(topic, qos);
  
  if (_debug) {
    if (result) {
      debugPrint("SUBSCRIBE SUCCESS");
      debugPrint("  Topic: " + String(topic));
      debugPrint("  QoS: " + String(qos));
    } else {
      debugPrint("SUBSCRIBE FAILED");
      debugPrint("  Topic: " + String(topic));
    }
    
    // Simpan topic untuk resubscribe nanti
    String topicStr = String(topic);
    bool exists = false;
    for (const auto& existingTopic : _subscribedTopics) {
      if (existingTopic == topicStr) {
        exists = true;
        break;
      }
    }
    
    if (!exists) {
      _subscribedTopics.push_back(topicStr);
      debugPrint("  Subscription saved for auto-resubscribe");
    }
    debugPrint(""); // Spasi setelah subscribe
  }
}

const char* MQTTESP::getIncomingTopic() {
  return _incomingTopic;
}

const char* MQTTESP::getIncomingMessage() {
  return _incomingMessage;
}

void MQTTESP::setIncomingMessage(const char* message) {
  strncpy(_incomingMessage, message, sizeof(_incomingMessage) - 1);
  _incomingMessage[sizeof(_incomingMessage) - 1] = '\0';
}

void MQTTESP::setDebug(bool enable) {
  _debug = enable;
  debugPrint(""); // Spasi
  debugPrint("Debug " + String(enable ? "enabled" : "disabled"));
  debugPrint(""); // Spasi
}

bool MQTTESP::getDebug() {
  return _debug;
}

void MQTTESP::connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  
  debugPrint("Connecting to WiFi...");
  debugPrint("  SSID: " + String(_ssid));
  
  WiFi.begin(_ssid, _password);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    if (_debug) Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    debugPrint("WiFi connected");
    debugPrint("  IP: " + WiFi.localIP().toString());
    debugPrint("  RSSI: " + String(WiFi.RSSI()) + " dBm");
  } else {
    debugPrint("WiFi connection failed");
  }
  debugPrint(""); // Spasi setelah koneksi WiFi
}

void MQTTESP::connectMQTT() {
  if (_client.connected()) return;
  
  String clientId = "ESPClient-" + String(random(0xffff), HEX);
  debugPrint("Connecting to MQTT broker...");
  debugPrint("  Client ID: " + clientId);
  debugPrint("  Server: " + String(_mqtt_server));
  debugPrint("  Port: " + String(_mqtt_port));
  
  if (_mqtt_user && _mqtt_pass) {
    debugPrint("  Using authentication");
  } else {
    debugPrint("  No authentication");
  }
  
  bool connected = false;
  unsigned long startTime = millis();
  
  while (!connected && millis() - startTime < 10000) {
    if (_mqtt_user && _mqtt_pass) {
      connected = _client.connect(clientId.c_str(), _mqtt_user, _mqtt_pass);
    } else {
      connected = _client.connect(clientId.c_str());
    }
    
    if (!connected) {
      if (_debug) Serial.print(".");
      delay(1000);
    }
  }

  if (connected) {
    debugPrint("MQTT connected");
    
    // Subscribe ulang ke semua topik yang pernah di-subscribe
    resubscribeAll();
  } else {
    debugPrint("MQTT connection failed");
    debugPrint("  Error code: " + String(_client.lastError()));
  }
  debugPrint(""); // Spasi setelah koneksi MQTT
}

void MQTTESP::resubscribeAll() {
  if (_subscribedTopics.empty()) {
    debugPrint("No topics to resubscribe");
    return;
  }
  
  debugPrint("Auto-resubscribing to " + String(_subscribedTopics.size()) + " topics:");
  for (const auto& topic : _subscribedTopics) {
    bool result = _client.subscribe(topic.c_str());
    if (_debug) {
      if (result) {
        debugPrint("  Done " + topic);
      } else {
        debugPrint("  Fail " + topic + " (failed)");
      }
    }
  }
  debugPrint("Auto-resubscribe completed");
  debugPrint(""); // Spasi setelah resubscribe
}

void MQTTESP::onMessage(String &topic, String &payload) {
  strlcpy(_incomingTopic, topic.c_str(), sizeof(_incomingTopic));
  strlcpy(_incomingMessage, payload.c_str(), sizeof(_incomingMessage));
  
  if (_debug) {
    debugPrint("MESSAGE RECEIVED");
    debugPrint("  Topic: " + topic);
    debugPrint("  Payload: " + payload);
    debugPrint(""); // Spasi setelah menerima pesan
  }
}

void MQTTESP::debugPrint(String message) {
  if (_debug) {
    Serial.println(message);
  }
}
