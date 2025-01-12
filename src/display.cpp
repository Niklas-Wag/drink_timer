#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

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
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.print(text);
        display.display();
        lastText = text;
    }
}

void displayTimeResults(double time)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(time, 1);
    display.print("s ");
    display.display();
}

void displayVolumeResults(double volume)
{
    display.setCursor(0, 16);
    display.print(volume, 2);
    display.print("l");
    display.display();
}