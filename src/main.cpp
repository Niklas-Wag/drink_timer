#include <Arduino.h>
#include "display.h"
#include "scale.h"
#include "server.h"
#include "game.h"
#include "rotaryEncoderFunctions.h"

void setup()
{
  Serial.begin(115200);

  setupScale();
  setupDisplay();
  setupServer();
  setupRotary();
}

void loop()
{
  gameLoop();
}
