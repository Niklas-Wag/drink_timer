#include <RotaryEncoder.h>
#include "playerSelection.h"
#include "server.h"
#include "display.h"

#define CLK_PIN 19
#define DT_PIN 18
#define SW_PIN 5
#define DEBOUNCE_DELAY 50

RotaryEncoder encoder(CLK_PIN, DT_PIN, RotaryEncoder::LatchMode::FOUR3);

const char *names[] = {"Guest", "Julia", "Niklas", "Xaver", "Dominik", "Schauli", "Mike", "Annika", "Jonas"};
const int namesCount = sizeof(names) / sizeof(names[0]);
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
                currentIndex = namesCount - 1;
                encoder.setPosition(currentIndex);
            }
            else if (currentIndex >= namesCount)
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
                    return names[currentIndex];
                }
            }
        }

        lastButtonState = reading;
    }
}
