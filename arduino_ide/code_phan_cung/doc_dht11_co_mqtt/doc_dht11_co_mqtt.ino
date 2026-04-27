#include <ArduinoJson.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <WiFi.h>

// ===== WIFI =====
const char *ssid = "HAN TINH";
const char *password = "88888888";

// ===== MQTT =====
const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char *topic_pub = "iot/esp32c3_01/data";
const char *topic_ctrl = "esp32/dht11/control";
const char *topic_status = "esp32/dht11/status";

// ===== DHT11 =====
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== LED =====
#define LED_TEMP 9
#define LED_HUM 0

// ===== BIẾN TOÀN CỤC =====
WiFiClient espClient;
PubSubClient client(espClient);
bool autoMode = true;
bool manualLedTemp = false;
bool manualLedHum = false;
float currentTemp = 0;
float currentHum = 0;
unsigned long lastPublish = 0;
const long publishInterval = 2000;

// ===== CONNECT WIFI =====
void setup_wifi() {
  delay(1000);
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ===== MQTT CALLBACK =====
void callback(char *topic, byte *payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("MQTT received: ");
  Serial.println(message);

  if (String(topic) == topic_ctrl) {
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error)
      return;

    const char *command = doc["command"];
    const char *value = doc["value"];

    if (strcmp(command, "mode") == 0) {
      if (strcmp(value, "auto") == 0) {
        autoMode = true;
        Serial.println("Auto mode ON");
        updateLEDsBySensor();
      } else if (strcmp(value, "manual") == 0) {
        autoMode = false;
        Serial.println("Manual mode ON");
      }
    } else if (strcmp(command, "led") == 0) {
      if (!autoMode) {
        if (strcmp(value, "temp") == 0) {
          manualLedTemp = !manualLedTemp;
          digitalWrite(LED_TEMP, manualLedTemp);
        } else if (strcmp(value, "hum") == 0) {
          manualLedHum = !manualLedHum;
          digitalWrite(LED_HUM, manualLedHum);
        }
        publishStatus();
      }
    }
  }
}

// ===== UPDATE LED AUTO =====
void updateLEDsBySensor() {
  if (autoMode) {
    bool newLedTemp = (currentTemp > 30.0);
    bool newLedHum = (currentHum > 70.0);

    if (newLedTemp != manualLedTemp) {
      manualLedTemp = newLedTemp;
      digitalWrite(LED_TEMP, manualLedTemp);
    }
    if (newLedHum != manualLedHum) {
      manualLedHum = newLedHum;
      digitalWrite(LED_HUM, manualLedHum);
    }
  }
}

// ===== PUBLISH STATUS =====
void publishStatus() {
  StaticJsonDocument<128> doc;
  doc["led_hum"] = manualLedHum;
  doc["led_temp"] = manualLedTemp;
  doc["mode"] = autoMode ? "auto" : "manual";
  doc["timestamp"] = millis() / 1000;

  char buffer[128];
  serializeJson(doc, buffer);
  client.publish(topic_status, buffer);
}

// ===== RECONNECT MQTT =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    String clientId = "esp32c3_" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("OK");
      client.subscribe(topic_ctrl);
      publishStatus();
    } else {
      Serial.print("Fail, rc=");
      Serial.print(client.state());
      Serial.println(" retry...");
      delay(2000);
    }
  }
}

// ===== PUBLISH SENSOR  =====
void publishData(float temp, float hum) {
  StaticJsonDocument<256> doc;

  // Thông tin cơ bản
  doc["device_id"] = "esp32_c3_super_mini";
  doc["timestamp"] = millis() / 1000;

  // Dữ liệu cảm biến
  doc["temperature"] = temp;
  doc["humidity"] = hum;

  // ===== TRẠNG THÁI LED =====
  doc["led_temp_state"] = manualLedTemp; // true = BẬT, false = TẮT
  doc["led_hum_state"] = manualLedHum;   // true = BẬT, false = TẮT

  // Chế độ hiện tại
  doc["mode"] = autoMode ? "auto" : "manual";

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(topic_pub, buffer);
  Serial.print("Publish: ");
  Serial.println(buffer);
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  pinMode(LED_TEMP, OUTPUT);
  pinMode(LED_HUM, OUTPUT);
  digitalWrite(LED_TEMP, LOW);
  digitalWrite(LED_HUM, LOW);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ===== LOOP =====
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - lastPublish >= publishInterval) {
    lastPublish = millis();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT11 read failed!");
      return;
    }

    currentTemp = temp;
    currentHum = hum;

    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" C | Hum: ");
    Serial.print(hum);
    Serial.println(" %");
    Serial.print("LED Temp: ");
    Serial.print(manualLedTemp ? "ON" : "OFF");
    Serial.print(" | LED Hum: ");
    Serial.println(manualLedHum ? "ON" : "OFF");

    if (autoMode) {
      updateLEDsBySensor();
    }

    publishData(temp, hum);
    publishStatus();
  }
  delay(100);
}