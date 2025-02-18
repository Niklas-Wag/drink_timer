#include "game.h"
#include "display.h"
#include "scale.h"
#include "server.h"
#include <RotaryEncoder.h>
#include "rotaryEncoderFunctions.h"
#include <list>

enum GameState
{
    CHOOSE_PLAYER,
    WAIT_FOR_GLASS,
    WAIT_FOR_DRINKING,
    DRINKING,
    SHOW_RESULTS
};
GameState currentState = CHOOSE_PLAYER;

float initialWeight = 0;
float drunkWeight = -1;
unsigned long startTime = 0;
unsigned long timer = 0;
unsigned long resultStartTime = 0;
String player = "";

int currentIdx = 0;
bool firstView = true;
#define CLK_PIN 19
#define DT_PIN 18
String screens[3][2];

void gameLoop()
{
    double weight = getWeight();
    switch (currentState)
    {
    case CHOOSE_PLAYER:
        player = getPlayer();
        currentState = WAIT_FOR_GLASS;
        break;
    case WAIT_FOR_GLASS:
        displayMultipleTexts({"Place", "glass!"});
        initialWeight = waitForStableWeight();
        currentState = WAIT_FOR_DRINKING;
        break;
    case WAIT_FOR_DRINKING:
        displayText("Drink!");
        if (weight < initialWeight - 50)
        {
            startTime = millis();
            currentState = DRINKING;
            while (weight >= 50)
            {
                weight = getWeight();
            }
        }
        break;
    case DRINKING:
        timer = millis() - startTime;
        displayText(String(timer / 1000) + "s");
        if (weight >= 50)
        {
            currentState = SHOW_RESULTS;
        }
        break;
    case SHOW_RESULTS:
        if (drunkWeight == -1)
        {
            String rank = "N/A";
            String roundedTime = String(round((timer / 1000.0) * 10) / 10, 1) + " s";
            displayText(roundedTime);
            drunkWeight = initialWeight - waitForStableWeight();
            if (drunkWeight < 0)
            {
                drunkWeight = 0;
            }
            
            screens[0][0] = roundedTime;
            screens[0][1] = String(drunkWeight / 1000.0, 2) + " l";
            
            if (drunkWeight > 5 && player != "Guest")
            {
                rank = String(addEntry(player, timer, drunkWeight));
            }

            String category = "N/A";
            if (0.25 <= drunkWeight && drunkWeight <= 0.35)
            {
                category = "Seidl";
            }
            else if (0.45 <= drunkWeight && drunkWeight <= 0.6)
            {
                category = "Hoibe";
            }
            else
            {
                category = "Other";
            }

            screens[1][0] = category + ":";
            screens[1][1] = String(rank) + ".";

            firstView = false;
        }

        while (weight >= 50)
        {
            serverHandleClient();
            weight = getWeight();
            unsigned long elapsed = millis() - resultStartTime;
            
            if (elapsed > 2000) // Switch every 2 seconds
            {
                firstView = !firstView;
                resultStartTime = millis();
                displayMultipleTexts({screens[firstView ? 0 : 1][0], screens[firstView ? 0 : 1][1]});
            }
        }

        currentState = CHOOSE_PLAYER;
        drunkWeight = -1;
        break;
    }
}
