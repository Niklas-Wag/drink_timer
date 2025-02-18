#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>

void setupScale();
double getWeight();
double waitForStableWeight(double stabilityThreshold = 5.0);

#endif
