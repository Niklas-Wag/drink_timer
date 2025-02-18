#include <RotaryEncoder.h>
#include "rotaryEncoderFunctions.h"
#include "server.h"
#include "display.h"
#include "server.h"
#include <vector>
#include <string>

#define CLK_PIN 19
#define DT_PIN 18
#define SW_PIN 5
#define DEBOUNCE_DELAY 50

RotaryEncoder encoder(CLK_PIN, DT_PIN, RotaryEncoder::LatchMode::FOUR3);

int currentIndex = 0;

void displayCurrentName()
{
    displayMultipleTexts({"Player:", names[currentIndex]});
}

void setupRotary()
{
    pinMode(SW_PIN, INPUT_PULLUP);
    encoder.setPosition(0);
}

String getPlayer()
{
    unsigned long lastDebounceTime = 0;
    bool lastButtonState = HIGH;
    bool buttonState = HIGH;
    displayCurrentName();

    while (true)
    {
        serverHandleClient();
        encoder.tick();

        int newPosition = encoder.getPosition();
        if (newPosition != currentIndex)
        {
            currentIndex = newPosition;
            if (currentIndex < 0)
            {
                currentIndex = names.size() - 1; // Use names.size() instead of namesCount
                encoder.setPosition(currentIndex);
            }
            else if (currentIndex >= names.size()) // Use names.size() instead of namesCount
            {
                currentIndex = 0;
                encoder.setPosition(currentIndex);
            }
            displayCurrentName();
        }

        bool reading = digitalRead(SW_PIN);
        if (reading != lastButtonState)
        {
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
        {
            if (reading != buttonState)
            {
                buttonState = reading;
                if (buttonState == LOW)
                {
                    return names[currentIndex]; // Return the selected name as a std::string
                }
            }
        }

        lastButtonState = reading;
    }
}