#include "scale.h"
#include "HX711.h"
#include "server.h"

#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 17

HX711 scale;

void setupScale()
{
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(-706.4440994);
    scale.tare();
}

double getWeight()
{
    double weight = scale.get_units(5);
    return (weight < 0) ? 0 : weight;
}

double waitForStableWeight(unsigned long timeout, double stabilityThreshold)
{
    unsigned long startTime = millis();
    double weight = getWeight();
    double previousWeight = weight;
    int stableCount = 0;

    while (true)
    {
        // quick hack to keep the server running
        serverHandleClient();

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