#include "game.h"
#include "display.h"
#include "scale.h"
#include "server.h"
#include "playerSelection.h"

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
        displayText("Place your glass...");
        initialWeight = waitForStableWeight();
        currentState = WAIT_FOR_DRINKING;
        break;
    case WAIT_FOR_DRINKING:
        displayText("Start drinking...");
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
            displayTimeResults(timer / 1000.0);
            drunkWeight = initialWeight - waitForStableWeight();
            displayVolumeResults(drunkWeight / 1000.0);
            if (drunkWeight > 5 && player != "Guest")
            {
                addEntry(player, timer, drunkWeight);
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
