#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>

void setupScale();
double getWeight();
double waitForStableWeight(unsigned long timeout = 5000, double stabilityThreshold = 5.0);

#endif // SCALE_H
