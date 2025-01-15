#ifndef SERVER_H
#define SERVER_H

#include <Arduino.h>

void setupServer();
void serverHandleClient();
void addEntry(const String &name, int time, int quantity);

#endif
