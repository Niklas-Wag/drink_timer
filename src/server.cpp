#include "server.h"
#include <SPIFFS.h>
#include <WebServer.h>

WebServer server(80);

String generateLeaderboard(double minL, double maxL)
{
    File file = SPIFFS.open("/leaderboard.txt", FILE_READ);
    if (!file)
    {
        Serial.println("Error opening leaderboard file.");
        return "<p>Error loading leaderboard.</p>";
    }

    struct Entry
    {
        String name;
        double liters;
        double time;
        float lPerS;
    };

    std::vector<Entry> entries;

    while (file.available())
    {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() == 0)
            continue;

        int comma1 = line.indexOf(',');
        int comma2 = line.indexOf(',', comma1 + 1);

        if (comma1 == -1 || comma2 == -1)
            continue;

        String name = line.substring(0, comma1);
        double seconds = line.substring(comma1 + 1, comma2).toDouble() / 1000.0;
        double liters = line.substring(comma2 + 1).toDouble() / 1000.0;

        if (liters >= minL && liters <= maxL && seconds > 0)
        {
            float lPerS = (float)liters / seconds;
            entries.push_back({name, liters, seconds, lPerS});
        }
    }

    file.close();

    std::sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b)
              { return a.lPerS > b.lPerS; });

    String tableRows;
    for (const auto &entry : entries)
    {
        tableRows += "<tr>";
        tableRows += "<td>" + entry.name + "</td>";
        tableRows += "<td>" + String(entry.liters) + "</td>";
        tableRows += "<td>" + String(entry.time) + "</td>";
        tableRows += "<td>" + String(entry.lPerS, 2) + "</td>";
        tableRows += "</tr>";
    }

    return tableRows;
}

void handleRoot()
{
    File file = SPIFFS.open("/index.html", FILE_READ);
    if (!file)
    {
        Serial.println("Error opening index.html file.");
        server.send(500, "text/plain", "Internal Server Error: Unable to load page.");
        return;
    }

    String html = file.readString();
    file.close();

    html.replace("{{LEADERBOARD_025_035}}", generateLeaderboard(0.25, 0.35));
    html.replace("{{LEADERBOARD_045_055}}", generateLeaderboard(0.45, 0.55));
    html.replace("{{LEADERBOARD_OTHER}}", generateLeaderboard(0, 1000000));

    server.send(200, "text/html", html);
}

void addEntry(const String &name, int time, int quantity)
{
    File file = SPIFFS.open("/leaderboard.txt", FILE_APPEND);
    if (!file)
    {
        Serial.println("Error opening leaderboard file.");
        return;
    }
    file.println(name + "," + String(time) + "," + String(quantity));
    file.close();
}

void setupServer()
{
    WiFi.softAP("ESP32", "12345678");
    server.on("/", handleRoot);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    server.begin();

    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
}

void serverHandleClient()
{
    server.handleClient();
}
