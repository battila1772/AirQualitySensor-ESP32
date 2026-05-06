#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <SparkFun_ENS160.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <WebServer.h>

#define SDA_PIN 21     //I2C pins
#define SCL_PIN 22     //

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

//---------------DEVICES-----------------

SparkFun_ENS160 airQualitySensor;
Adafruit_AHTX0 temperaturSensor;

Adafruit_SH1106G oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//--------------VARIABLES----------------

bool aht_ok = false;
bool ens_ok = false;

uint8_t airQualityIndex = 0;
uint16_t tvoc = 0;
uint16_t eco2 = 0;

float temperature = 0;
float humidity = 0;

unsigned long lastUpdate = 0;
const unsigned long interval = 1000;     //Display refresh rate 1s (millis)

const char* staSsid = "WiFi_Name";
const char* staPassword = "Password";

const char* apSsid = "AirQualityESP";
const char* apPassword = "12345678";

WebServer server(80);

//---------------PROTOTYPES--------------

void initSensors();
void initDisplay();
void connectWiFi();
void showLoadingAnimation();
void readTemperaturHumidity();
void readAirQuality();
void updateDisplay();
void handleData();
void handleRoot();

void iconTemperature(int x, int y);
void iconHumidity(int x, int y);
void drawAQIFace(int x, int y);

//------------------MAIN------------------

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);

  initDisplay();
  initSensors();
  showLoadingAnimation();
  connectWiFi();
}

//----------------LOOP--------------------

void loop() {

  server.handleClient(); 

  unsigned long now = millis();

  if (now - lastUpdate >= interval) {
    lastUpdate = now;

    readTemperaturHumidity();
    readAirQuality();
    updateDisplay();
  }
}

//---------------FUNCTIONS----------------

//-----------INITS------------

void initSensors() {
  aht_ok = temperaturSensor.begin();

  Serial.print("AHT: ");
  Serial.println(aht_ok ? "OK" : "FAIL");

  ens_ok = airQualitySensor.begin(Wire, 0x53);

  Serial.print("ENS160: ");
  Serial.println(ens_ok ? "OK" : "FAIL");

  delay(1000);

  if (ens_ok) {
    airQualitySensor.setOperatingMode(SFE_ENS160_STANDARD);
    delay(2000);                                              //Waiting for ENS initializiert and warms up
  }
}

void initDisplay() {
  if (!oled.begin(SCREEN_ADDRESS, true)) {
    while (1) delay(1000);
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SH110X_WHITE);
  oled.setCursor(0, 0);
  oled.println("Loading...");
  oled.display();

  delay(1000);
}

void connectWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(staSsid, staPassword);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 10000) {

    delay(500);
  }

  // -------- STA MODE --------
  if (WiFi.status() == WL_CONNECTED) {

    Serial.print("STA IP: ");                 //Types IP to Serial-monitor
    Serial.println(WiFi.localIP());          
  }

  // -------- AP MODE --------
  else {

    WiFi.mode(WIFI_AP);

    WiFi.softAP(apSsid, apPassword);

    Serial.print("AP IP: ");                   //IP: http://192.168.4.1/
    Serial.println(WiFi.softAPIP());
  }

  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();
}

void showLoadingAnimation() {
  int radius = 2;
  int direction = 1;

  unsigned long startTime = millis();

  while (millis() - startTime < 5000) {
    oled.clearDisplay();
    oled.fillCircle(64, 32, radius, SH110X_WHITE);
    oled.display();

    radius += direction;

    if (radius >= 10) direction = -1;
    if (radius <= 2) direction = 1;

    delay(40);
  }
}

//-----------GET_DATA-----------

void readTemperaturHumidity() {
  if (!aht_ok) return;

  sensors_event_t hum, temp;
  temperaturSensor.getEvent(&hum, &temp);

  temperature = temp.temperature;
  humidity = hum.relative_humidity;
}

void readAirQuality() {
  if (!ens_ok) return;

  if(aht_ok) {
    airQualitySensor.setTempCompensation(temperature);    //temparature and humidity compensation from aht to ens
    airQualitySensor.setRHCompensation(humidity);         //
  }


  if (airQualitySensor.checkDataStatus()) {
    airQualityIndex = airQualitySensor.getAQI();
    tvoc = airQualitySensor.getTVOC();
    eco2 = airQualitySensor.getECO2();
  }
}

//-----------DISPLAY-------------

void updateDisplay() {
  oled.clearDisplay();

  oled.setTextSize(2);
  oled.setCursor(10, 0);
  oled.print("AQI ");
  oled.print(airQualityIndex);

  oled.setTextSize(1);

  drawAQIFace(100, 12);

  oled.setCursor(0, 25);
  oled.print("VOC: ");
  oled.print(tvoc);
  oled.print(" ppb");

  oled.setCursor(0, 35);
  oled.print("CO2: ");
  oled.print(eco2);
  oled.print(" ppm");

  iconTemperature(0, 52);
  oled.setCursor(10, 52);
  oled.print(temperature, 1);
  oled.write(248);
  oled.print("C");

  iconHumidity(64, 52);
  oled.setCursor(74, 52);
  oled.print(humidity, 0);
  oled.print("%");

  oled.display();
}

//-----------ICONS-----------

void iconTemperature(int x, int y) {
  oled.drawLine(x + 2, y, x + 2, y + 6, SH110X_WHITE);
  oled.drawCircle(x + 2, y + 8, 2, SH110X_WHITE);
}

void iconHumidity(int x, int y) {
  oled.drawPixel(x + 2, y, SH110X_WHITE);
  oled.drawPixel(x + 1, y + 1, SH110X_WHITE);
  oled.drawPixel(x + 3, y + 1, SH110X_WHITE);
  oled.drawPixel(x, y + 2, SH110X_WHITE);
  oled.drawPixel(x + 4, y + 2, SH110X_WHITE);
  oled.drawPixel(x + 1, y + 3, SH110X_WHITE);
  oled.drawPixel(x + 2, y + 4, SH110X_WHITE);
  oled.drawPixel(x + 3, y + 3, SH110X_WHITE);
}

void drawAQIFace(int x, int y) {

  int r = 10;

  oled.drawCircle(x, y, r, SH110X_WHITE);

  // eyes
  oled.fillCircle(x - 3, y - 3, 1, SH110X_WHITE);
  oled.fillCircle(x + 3, y - 3, 1, SH110X_WHITE);

  if (airQualityIndex == 1) {
    // smile
    oled.drawPixel(x - 4, y + 3, SH110X_WHITE);
    oled.drawPixel(x - 3, y + 4, SH110X_WHITE);
    oled.drawPixel(x - 2, y + 5, SH110X_WHITE);
    oled.drawPixel(x - 1, y + 5, SH110X_WHITE);
    oled.drawPixel(x,     y + 5, SH110X_WHITE);
    oled.drawPixel(x + 1, y + 5, SH110X_WHITE);
    oled.drawPixel(x + 2, y + 5, SH110X_WHITE);
    oled.drawPixel(x + 3, y + 4, SH110X_WHITE);
    oled.drawPixel(x + 4, y + 3, SH110X_WHITE);
  } 
  else if (airQualityIndex == 2) {
    // meh
    oled.drawLine(x - 4, y + 4, x + 4, y + 4, SH110X_WHITE);
  } 
  else {
    // sad
    oled.drawPixel(x - 4, y + 5, SH110X_WHITE);
    oled.drawPixel(x - 3, y + 4, SH110X_WHITE);
    oled.drawPixel(x - 2, y + 3, SH110X_WHITE);
    oled.drawPixel(x - 1, y + 3, SH110X_WHITE);
    oled.drawPixel(x,     y + 3, SH110X_WHITE);
    oled.drawPixel(x + 1, y + 3, SH110X_WHITE);
    oled.drawPixel(x + 2, y + 3, SH110X_WHITE);
    oled.drawPixel(x + 3, y + 4, SH110X_WHITE);
    oled.drawPixel(x + 4, y + 5, SH110X_WHITE);
  }
}

  //-----------WiFI-------------

  void handleData() {

  String json = "{";
  json += "\"aqi\":" + String(airQualityIndex) + ",";
  json += "\"tvoc\":" + String(tvoc) + ",";
  json += "\"eco2\":" + String(eco2) + ",";
  json += "\"temp\":" + String(temperature, 1) + ",";
  json += "\"hum\":" + String(humidity, 0);
  json += "}";

  server.send(200, "application/json", json);
}

void handleRoot() {

  String page = "<h1>Air Quality Monitor</h1>";
  page += "<p>AQI: " + String(airQualityIndex) + "</p>";
  page += "<p>VOC: " + String(tvoc) + "</p>";
  page += "<p>CO2: " + String(eco2) + "</p>";
  page += "<p>Temperature: " + String(temperature, 1) + "</p>";
  page += "<p>Humidity: " + String(humidity, 0) + "</p>";

  server.send(200, "text/html", page);
}
