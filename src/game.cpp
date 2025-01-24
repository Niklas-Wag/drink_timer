#include "game.h"
#include "display.h"
#include "scale.h"
#include "server.h"
#include <RotaryEncoder.h>
#include "playerSelection.h"
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
String player = "";

int currentIdx = 0;
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
            displayText(String(timer / 1000.0, 2) + " s");
            drunkWeight = initialWeight - waitForStableWeight();
            if (drunkWeight < 0)
            {
                drunkWeight = 0;
            }
            displayMultipleTexts({String(timer / 1000.0, 2) + " s", String(drunkWeight / 1000.0, 2) + " l"});
            if (drunkWeight > 5 && player != "Guest")
            {
                rank = String(addEntry(player, timer, drunkWeight));
            }

            screens[0][0] = String(timer / 1000.0, 2) + " s";
            screens[0][1] = String(drunkWeight / 1000.0, 2) + " l";

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

            screens[2][0] = "l/s:";
            screens[2][1] = String(drunkWeight / timer, 2);

            encoder.setPosition(0);
        }

        while (true)
        {
            serverHandleClient();
            encoder.tick();
            int newPosition = abs(encoder.getPosition() % 3);
            if (newPosition != currentIdx)
            {
                currentIdx = newPosition;
                displayMultipleTexts({screens[currentIdx][0], screens[currentIdx][1]});
            }
        }

        if (weight < 50)
        {
            currentState = CHOOSE_PLAYER;
            drunkWeight = -1;
        }
        break;
    }
}
