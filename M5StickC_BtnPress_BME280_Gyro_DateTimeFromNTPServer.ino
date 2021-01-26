// Implementation of Button press function. Menu is displayed at first.
// If power button is pressed then Time and date values will displayed.
// If Button A (Home Button) is pressed then BME280 value is displayed.
// If Button B (Right button is pressed then Gyroscope is displayed.

// Date and Time libraries
#include "WiFi.h"
#include "time.h"

// M5 Stick Libraries
#include <M5StickC.h>
#include <Wire.h>

//For BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;

// WiFi connection credentials
const char* ssid = "put_your_wifi_ssid_here";
const char* password = "put_your_wifi_password";

// NTP Server credentials
const char* ntpServer = "ntp.jst.mfeed.ad.jp";
const long gmtOffset_sec = 9 * 3600;            // Japan time is GMT +9
const int daylightOffset_sec = 0;
struct tm timeinfo;

//For LED
int PIN = 32;

//For Gyroscope
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;


void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Wire.begin(0, 26);
  Serial.begin(115200);
  M5.IMU.Init();

  // WiFi connection setup
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%Y/%m/%d %a %H:%M:%S");

  // Disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // BME Connection
  while (!bme.begin()) {
    M5.Lcd.println("Error! Check wiring!");
  }

  // Displaying Menu on M5 LCD.
  M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.println("Press \n Pwr Btn: Time \n Btn A: BME280 \n Btn B: Gyroscope");
}

void loop() {
  // put your main code here, to run repeatedly
  M5.update();


  if (M5.BtnA.isPressed()) {      // If the home button (Button A), big button on the middle is pressed then tmeperature, pressure and humidity value is shown but you have
    while (1) {

      pinMode(GPIO_NUM_10, OUTPUT);
      digitalWrite(GPIO_NUM_10, LOW);

      M5.Lcd.fillScreen(BLACK);    
      
      Bme280();

      pinMode(GPIO_NUM_10, OUTPUT);
      digitalWrite(GPIO_NUM_10, HIGH);

      loop();
    }
  }

  if (M5.BtnB.isPressed()) {    // If the Button B (On the right side middle) is pressed then gyroscope value is shown but you have to
    while (1) {
      M5.Lcd.fillScreen(BLACK);   
      M5.Lcd.setCursor(0, 0);

      pinMode(GPIO_NUM_10, OUTPUT);
      digitalWrite(GPIO_NUM_10, LOW);

      Gyroscope();
      loop();
    }
  }

  if (M5.Axp.GetBtnPress()) {     // Pressing the power button on the left down side shows you date and time
    while (1) {
      pinMode(GPIO_NUM_10, OUTPUT);
      digitalWrite(GPIO_NUM_10, HIGH);
      
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setRotation(3);
      M5.Lcd.setCursor(15, 10);
      getTime();
      loop();
    }
  }
  delay(1000);
}

void Bme280() {
  M5.Lcd.fillScreen(BLACK);
  getTempC();
  getPressureP();
  getHumidityR();

}

void Gyroscope() {

  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(40, 0);
  M5.Lcd.println("SH200I TEST");
  M5.Lcd.setCursor(0, 15);
  M5.Lcd.println("  X       Y       Z");

  float temp = 0;
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  M5.IMU.getTempData(&temp);

  M5.Lcd.setCursor(0, 30);
  M5.Lcd.printf("%6.2f  %6.2f  %6.2f      ", gyroX, gyroY, gyroZ);
  M5.Lcd.setCursor(140, 30);
  M5.Lcd.print("o/s");
  M5.Lcd.setCursor(0, 45);
  M5.Lcd.printf(" %5.2f  %5.2f  %5.2f   ", accX, accY, accZ);
  M5.Lcd.setCursor(140, 45);
  M5.Lcd.print("G");
  M5.Lcd.setCursor(0, 60);
  M5.Lcd.printf("Temperature : %.2f C", temp);

}

void getTime() {
  getLocalTime(&timeinfo);        // Updating the date and time values in every loop run
  M5.Lcd.setTextColor(RED, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println(&timeinfo, "%Y/%m/%d \n");
  
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.println(&timeinfo, "%H:%M:%S");

}

void getTempC() {
  M5.Lcd.setCursor(4, 0);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println(" ");
  M5.Lcd.print("T:");
  M5.Lcd.print(bme.readTemperature());
  M5.Lcd.println(" *C");
}

void getPressureP() {
  M5.Lcd.setCursor(4, 38);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("P:");
  M5.Lcd.print(bme.readPressure() / 100.0F);
  M5.Lcd.println(" hPa");
}

void getHumidityR() {
  M5.Lcd.setCursor(4, 56);
  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("H:");
  M5.Lcd.print(bme.readHumidity());
  M5.Lcd.println(" %");
  M5.Lcd.println(" ");
}
