#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>

// Cấu hình thông tin WiFi và API key, tọa độ địa lý
const char* ssid = "TEKY OFFICE";      // Tên mạng WiFi
const char* password = "Teky@2018";      // Mật khẩu WiFi
String api = "https://api.openweathermap.org/data/2.5/weather?lat=10.7516584&lon=106.6261946&appid=039ed426e3299d943a18e69037191b87";

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);  // Khởi tạo Serial monitor ở tốc độ 115200 baud
  lcd.init();
  lcd.backlight();

  // Kết nối WiFi
  Serial.print("Connecting to WiFi...");
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  lcd.clear();
  lcd.print("WiFi Connected");
  delay(2000);
  lcd.clear();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    // Khởi tạo client HTTPS và vô hiệu hóa xác thực SSL để tránh lỗi chứng chỉ
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;
    Serial.println("Calling API...");

    // Mở kết nối tới API OpenWeather
    https.begin(client, api);
    int httpCode = https.GET();

    if (httpCode > 0) {
      // Kiểm tra mã trả về HTTP
      Serial.printf("HTTP Code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = https.getString();
        Serial.println("Payload: ");
        Serial.println(payload);

        // Phân tích chuỗi JSON
        JSONVar myObject = JSON.parse(payload);
        if (JSON.typeof(myObject) == "undefined") {
          Serial.println("JSON parse failed!");
          lcd.clear();
          lcd.print("JSON error!");
          https.end();
          return;
        }

        // Lấy nhiệt độ (đơn vị Kelvin) từ trường "main.temp"
        float tempK = double(myObject["main"]["temp"]);
        // Chuyển đổi Kelvin sang Celsius
        float tempC = tempK - 273.15;
        String weather = String(myObject["weather"][0]["main"]);
        Serial.print("Temperature: ");
        Serial.print(tempC);
        Serial.println(" C");

        // Hiển thị nhiệt độ lên LCD
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Temp:");
        lcd.print(tempC, 1); // hiển thị 1 chữ số sau dấu phẩy
        lcd.print(" C");
        lcd.setCursor(0, 0);
        lcd.print("Weather: ");
        lcd.print(weather);

        
      }
    } else {
      Serial.printf("HTTP request error: %s\n", https.errorToString(httpCode).c_str());
      lcd.clear();
      lcd.print("HTTP error");
    }
    https.end();
  } else {
    Serial.println("WiFi not connected");
    lcd.clear();
    lcd.print("No WiFi");
  }
  delay(10000);  // Đợi 10 giây trước khi gọi API lần tiếp theo
}
