#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32_AP";
const char *password = "12345678"; // Minimum 8 characters

// Set up a WebServer on port 80
WebServer server(80);

// HTML content for the webpage
const char htmlPage[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Web Server</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    h1 { color: #005f5f; }
  </style>
</head>
<body>
  <h1>ESP32 Web Server</h1>
  <p>Welcome to your first ESP32 webpage!</p>
</body>
</html>
)rawliteral";

// Handle root URL
void handleRoot()
{
  server.send(200, "text/html", htmlPage);
}

#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
HX711 scale;

enum GameState
{
  WAIT_FOR_GLASS,
  WAIT_FOR_DRINKING,
  DRINKING,
  SHOW_RESULTS
};

GameState currentState = WAIT_FOR_GLASS;

float initailWeight = 0;
float drunkWeight = 0;
unsigned long startTime = 0;
unsigned long timer = 0;

double weight = 0;
unsigned long lastWeightRead = 0;
unsigned long lastDisplayUpdate = 0;

double getWeight()
{
  double weight = scale.get_units(5);
  if (weight < 0)
    weight = 0;
  return weight;
}

void displayText(String text)
{
  static String lastText = "";
  if (text != lastText)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(text);
    display.display();
    lastText = text;
  }
}

double waitForStableWeight(unsigned long timeout = 5000, double stabilityThreshold = 5.0)
{
  unsigned long startTime = millis();
  double weight = getWeight();
  double previousWeight = weight;
  int stableCount = 0;

  while (true)
  {

    server.handleClient();
    weight = getWeight();
    if (abs(weight - previousWeight) <= stabilityThreshold && weight > 20)
    {
      stableCount++;
      if (stableCount >= 3)
      {
        return weight;
      }
    }
    else
    {
      stableCount = 0;
    }

    previousWeight = weight;
  }
}

void setup()
{
  Serial.begin(115200);

  rtc_cpu_freq_config_t config;
  rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_160M, &config);
  rtc_clk_cpu_freq_set_config_fast(&config);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-706.9184549);
  scale.tare();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();

  Serial.println("Setting up Access Point...");
  WiFi.softAP(ssid, password);

  // Print IP address of the Access Point
  Serial.println("Access Point set up!");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Start the web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started.");
}

void loop()
{
  server.handleClient();

  unsigned long now = millis();

  if (now - lastWeightRead >= 100)
  {
    weight = getWeight();
    lastWeightRead = now;
  }

  switch (currentState)
  {
  case WAIT_FOR_GLASS:
    displayText("Place your glass...");
    initailWeight = waitForStableWeight();
    currentState = WAIT_FOR_DRINKING;
    break;

  case WAIT_FOR_DRINKING:
    displayText("Start drinking...");
    if (weight < initailWeight - 50)
    {
      startTime = now;
      currentState = DRINKING;
    }
    break;

  case DRINKING:
    timer = now - startTime;
    if (now - lastDisplayUpdate >= 200)
    {
      displayText(String(timer / 1000) + "s");
      lastDisplayUpdate = now;
    }
    if (weight >= 50)
    {
      currentState = SHOW_RESULTS;
    }
    break;

  case SHOW_RESULTS:
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(timer / 1000.0, 1);
    display.print("s ");
    display.display();

    drunkWeight = initailWeight - waitForStableWeight();

    display.setCursor(0, 16);
    display.print(drunkWeight / 1000.0, 2);
    display.print("l");
    display.display();

    while (getWeight() > 50)
    {
      server.handleClient();
      delay(100);
    }

    currentState = WAIT_FOR_GLASS;
    break;
  }
}
