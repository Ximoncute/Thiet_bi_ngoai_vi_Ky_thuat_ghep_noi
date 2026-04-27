#include "DHT.h"

#define DHTPIN 8       // GPIO8
#define DHTTYPE DHT11

#define LED_HUM 1       // GPIO1
#define LED_TEMP 3      // GPIO3

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  pinMode(LED_HUM, OUTPUT);
  pinMode(LED_TEMP, OUTPUT);

  dht.begin();

  Serial.println("Start DHT11 + ESP32-C3");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Kiểm tra lỗi đọc
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Loi doc DHT11!");
    delay(500);
    return;
  }

  // In ra Serial Monitor
  Serial.print("Nhiet do: ");
  Serial.print(temperature);
  Serial.print(" °C | Do am: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Điều khiển LED theo điều kiện
  if (humidity > 65) {
    digitalWrite(LED_HUM, HIGH);
  } else {
    digitalWrite(LED_HUM, LOW);
  }

  if (temperature > 27) {
    digitalWrite(LED_TEMP, HIGH);
  } else {
    digitalWrite(LED_TEMP, LOW);
  }

  delay(500); // DHT11 đọc mỗi 0,5s
}