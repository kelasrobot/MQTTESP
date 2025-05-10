# MQTTESP

MQTTESP adalah library yang menyederhanakan penggunaan dari library [`MQTT.h`](https://github.com/256dpi/arduino-mqtt). Oleh karena itu, Anda harus **menginstal `MQTT.h` terlebih dahulu** sebelum menggunakan MQTTESP.

MQTTESP membungkus fungsi-fungsi dasar dari `MQTT.h` dan menyediakan pendekatan yang lebih praktis untuk penggunaan MQTT di ESP8266/ESP32. Library ini membantu Anda menghindari kode duplikat seperti koneksi WiFi, koneksi ulang MQTT, dan pengecekan pesan masuk.

---

## ✅ Fitur Utama

* 📡 **Auto Connect WiFi & MQTT**
* 🔁 **Auto Reconnect saat koneksi terputus**
* 🔔 **Callback per topik untuk respon instan**
* 📨 **Publish & Subscribe dengan QoS dan Retain**
* 🧠 **Simpan dan baca pesan terakhir dengan `getIncomingMessage()`**
* 🧩 **Kompatibel dengan MQTT 3.1.1 dan MQTT 5**
* 🛠️ **Mudah diintegrasikan dalam proyek IoT apapun**

---

## 🔧 Instalasi

1. **Install MQTT.h**:
   Unduh dari [https://github.com/256dpi/arduino-mqtt](https://github.com/256dpi/arduino-mqtt)

2. **Install MQTTESP**:

   * Download atau clone library ini ke folder `libraries` Arduino Anda.

---

## 💡 Contoh Penggunaan

### 1. **Contoh Dasar: Publish & Subscribe**

```cpp
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
```

---

### 2. **Callback per Topik**

```cpp
void kontrolLampu(String pesan) {
  digitalWrite(2, pesan.toInt());
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);

  mqtt.begin();
  mqtt.subscribe(topicLampu, kontrolLampu);
}
```

---

### 3. **Publish dengan QoS dan Retain**

```cpp
mqtt.publish("tes/status", "ON", true, 1); // retain = true, QoS = 1
```

---

### 4. **Auto-Reconnect MQTT dan WiFi**

Library ini otomatis menghubungkan kembali jika WiFi atau broker MQTT terputus. Tidak perlu tambahan kode.

---

### 5. **Gunakan beberapa topik callback sekaligus**

```cpp
mqtt.subscribe("sensor/1", [](String pesan) {
  Serial.println("Sensor 1: " + pesan);
});

mqtt.subscribe("sensor/2", [](String pesan) {
  Serial.println("Sensor 2: " + pesan);
});
```

---

## 📁 Struktur File

```
MQTTESP/
├── examples/
│   └── Simple_Publish_and_Subscribe/
│       └── Simple_Publish_and_Subscribe.ino
├── src/
│   └── MQTTESP.h
│   └── MQTTESP.cpp
├── library.properties
└── README.md
```

---

## ✍️ Kontribusi

Silakan fork dan pull request jika ingin menambahkan fitur atau memperbaiki bug.