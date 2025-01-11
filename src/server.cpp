#include "server.h"
#include <WebServer.h>
#include <SPIFFS.h>

WebServer server(80);

void setupServer()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Initialization failed!");
        return;
    }
    server.on("/", []()
              {
        String html = "<html><body><h1>Drink Tracker</h1></body></html>";
        server.send(200, "text/html", html); });
    server.begin();
}

void serverHandleClient()
{
    server.handleClient();
}

void addEntry(const String &name, int time, int quantity)
{
    File file = SPIFFS.open("/leaderboard.txt", FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file.");
        return;
    }
    file.println(name + "," + String(time) + "," + String(quantity));
    file.close();
}
