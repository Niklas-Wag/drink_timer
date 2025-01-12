#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void setupDisplay();
void displayText(const String &text);
void displayTimeResults(double time);
void displayVolumeResults(double volume);

#endif // DISPLAY_H
