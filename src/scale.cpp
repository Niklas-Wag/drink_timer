#include "scale.h"
#include "HX711.h"
#include "server.h"

#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 17

HX711 scale;

void setupScale()
{
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(-706.2438017);
    delay(500);
    scale.tare();
}

double getWeight()
{
    double weight = scale.get_units(1);
    return (weight < 0) ? 0 : weight;
}

double waitForStableWeight(double stabilityThreshold)
{
    double previousWeight = getWeight();
    int stableCount = 0;

    while (true)
    {
        double weight = getWeight();
        if (abs(weight - previousWeight) <= stabilityThreshold && weight > 20)
        {
            stableCount++;
            if (stableCount >= 15)
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