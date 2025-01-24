#include "server.h"
#include <SPIFFS.h>
#include <WebServer.h>

WebServer server(80);

struct Entry
{
    String name;
    double liters;
    double time;
    float lPerS;
};

String getFilename(double liters)
{
    if (liters >= 0.25 && liters <= 0.35)
        return "/leaderboard_025_035.txt";
    else if (liters >= 0.45 && liters <= 0.6)
        return "/leaderboard_045_060.txt";
    else
        return "/leaderboard_other.txt";
}

String generateLeaderboard(const String &filename)
{
    File file = SPIFFS.open(filename, FILE_READ);
    if (!file)
    {
        Serial.println("Error opening leaderboard file.");
        return "<p>Error loading leaderboard.</p>";
    }

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

        if (seconds > 0)
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

    html.replace("{{LEADERBOARD_025_035}}", generateLeaderboard("/leaderboard_025_035.txt"));
    html.replace("{{LEADERBOARD_045_055}}", generateLeaderboard("/leaderboard_045_060.txt"));
    html.replace("{{LEADERBOARD_OTHER}}", generateLeaderboard("/leaderboard_other.txt"));

    server.send(200, "text/html", html);
}

int addEntry(const String &name, int time, int quantity)
{
    double liters = quantity / 1000.0;
    double seconds = time / 1000.0;
    if (seconds <= 0)
        return -1;

    String filename = getFilename(liters);
    File file = SPIFFS.open(filename, FILE_APPEND);
    if (!file)
    {
        Serial.println("Error opening leaderboard file.");
        return -1;
    }

    file.println(name + "," + String(time) + "," + String(quantity));
    file.close();

    file = SPIFFS.open(filename, FILE_READ);
    if (!file)
        return -1;

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

        String entryName = line.substring(0, comma1);
        double entrySeconds = line.substring(comma1 + 1, comma2).toDouble() / 1000.0;
        double entryLiters = line.substring(comma2 + 1).toDouble() / 1000.0;

        float lPerS = (float)entryLiters / entrySeconds;
        entries.push_back({entryName, entryLiters, entrySeconds, lPerS});
    }
    file.close();

    std::sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b)
              { return a.lPerS > b.lPerS; });

    for (int i = 0; i < entries.size(); i++)
    {
        if (entries[i].name == name && entries[i].liters == liters && entries[i].time == seconds)
            return i + 1;
    }

    return -1;
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
