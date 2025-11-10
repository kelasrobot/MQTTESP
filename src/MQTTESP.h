#ifndef MQTTESP_h
#define MQTTESP_h

#include <Arduino.h>
#include <WiFiClient.h>
#include <MQTT.h>
#include <vector>

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

class MQTTESP {
  public:
    MQTTESP(const char* ssid, const char* password, const char* mqtt_server,
            int mqtt_port = 1883, const char* mqtt_user = nullptr, const char* mqtt_pass = nullptr);
    
    void begin();
    void loop();
    bool isConnected();
    
    void publish(String topic, String payload);
    void publish(String topic, String payload, bool retained, int qos);
    
    void subscribe(const char* topic);
    void subscribe(const char* topic, int qos);
    
    // Methods for incoming messages and topics
    const char* getIncomingTopic();
    const char* getIncomingMessage();
    void setIncomingMessage(const char* message);

    // Debug control
    void setDebug(bool enable);
    bool getDebug();

  private:
    const char* _ssid;
    const char* _password;
    const char* _mqtt_server;
    const char* _mqtt_user;
    const char* _mqtt_pass;
    int _mqtt_port;
    bool _debug;
    
    WiFiClient _net;
    MQTTClient _client;

    // Buffers for incoming messages and topics
    char _incomingTopic[100];
    char _incomingMessage[100];

    // Simple subscription management
    std::vector<String> _subscribedTopics;

    void connectWiFi();
    void connectMQTT();
    void resubscribeAll();
    void onMessage(String &topic, String &payload);
    
    // Helper function for debug prints
    void debugPrint(String message);
};

#endif
