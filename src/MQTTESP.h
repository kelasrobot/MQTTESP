#ifndef MQTTESP_h
#define MQTTESP_h

#include <Arduino.h>
#include <WiFiClient.h>
#include <MQTT.h>

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
    
    // Callback system
    typedef void (*MQTTMessageCallback)(const char* topic, const char* payload);
    void registerCallback(const char* topic, MQTTMessageCallback callback);

    // Methods for incoming messages and topics
    const char* getIncomingTopic();
    const char* getIncomingMessage();
    void setIncomingTopic(const char* topic);
    void setIncomingMessage(const char* message);

  private:
    const char* _ssid;
    const char* _password;
    const char* _mqtt_server;
    const char* _mqtt_user;
    const char* _mqtt_pass;
    int _mqtt_port;
    
    WiFiClient _net;
    MQTTClient _client;

    // Buffers for incoming messages and topics
    char _incomingTopic[100];
    char _incomingMessage[100];

    // Callback management
    struct TopicCallback {
        String topicPattern;
        MQTTMessageCallback callback;
    };
    
    static const int MAX_CALLBACKS = 5;
    TopicCallback _callbacks[MAX_CALLBACKS];
    int _callbackCount = 0;

    void connectWiFi();
    void connectMQTT();
    static void onMessageStatic(String &topic, String &payload);
    void onMessage(String &topic, String &payload);
    static bool topicMatches(const char* pattern, const char* topic);
};

#endif
