#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setupDisplay()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    display.setTextColor(WHITE);
    display.clearDisplay();
    display.display();
}

void displayText(const String &text)
{
    static String lastText = "";
    if (text != lastText)
    {
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(0, 0);
        display.print(text);
        display.display();
        lastText = text;
    }
}

void displayMultipleTexts(const std::vector<String> &texts)
{
    display.clearDisplay();
    display.setTextSize(3);
    for (size_t i = 0; i < texts.size(); ++i)
    {
        display.setCursor(0, i * 32);
        display.print(texts[i]);
    }
    display.display();
}

void displayTimeResults(double time)
{
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(0, 0);
    display.print(time, 1);
    display.print("s ");
    display.display();
}

void displayVolumeResults(double volume)
{
    display.setCursor(0, 32);
    display.print(volume, 2);
    display.print("l");
    display.display();
}