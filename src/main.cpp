#include <Arduino.h>
#include "display.h"
#include "scale.h"
#include "server.h"
#include "game.h"

void setup()
{
  Serial.begin(115200);

  setupScale();
  setupDisplay();
  // setupServer();
}

void loop()
{
  // serverHandleClient();
  gameLoop();
}
