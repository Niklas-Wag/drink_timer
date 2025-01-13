#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <vector>

void setupDisplay();
void displayText(const String &text);
void displayMultipleTexts(const std::vector<String> &texts);
void displayTimeResults(double time);
void displayVolumeResults(double volume);

#endif // DISPLAY_H
