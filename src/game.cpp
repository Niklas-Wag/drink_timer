#include "game.h"
#include "display.h"
#include "scale.h"
#include "server.h"

enum GameState
{
    WAIT_FOR_GLASS,
    WAIT_FOR_DRINKING,
    DRINKING,
    SHOW_RESULTS
};
GameState currentState = WAIT_FOR_GLASS;

float initialWeight = 0;
float drunkWeight = -1;
unsigned long startTime = 0;
unsigned long timer = 0;

void gameLoop()
{
    double weight = getWeight();
    switch (currentState)
    {
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
            addEntry("Player", timer, drunkWeight);
        }
        if (weight < 50)
        {
            currentState = WAIT_FOR_GLASS;
        }
        break;
    }
}
