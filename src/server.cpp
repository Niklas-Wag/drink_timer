#include "server.h"
#include <SPIFFS.h>
#include <WebServer.h>

WebServer server(80);

String generateLeaderboard(int minML, int maxML)
{
    File file = SPIFFS.open("/leaderboard.txt", FILE_READ);
    if (!file)
    {
        Serial.println("Error opening file.");
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

        if (liters >= minML && liters <= maxML && seconds > 0)
        {
            float lPerS = (float)liters / seconds;
            entries.push_back({name, liters, seconds, lPerS});
        }
    }

    file.close();

    std::sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b)
              { return a.lPerS > b.lPerS; });

    String table = "<table>";
    table += "<tr><th>Player</th><th>liter</th><th>s</th><th>l/s</th></tr>";

    for (const auto &entry : entries)
    {
        table += "<tr>";
        table += "<td>" + entry.name + "</td>";
        table += "<td>" + String(entry.liters) + "</td>";
        table += "<td>" + String(entry.time) + "</td>";
        table += "<td>" + String(entry.lPerS, 2) + "</td>";
        table += "</tr>";
    }

    table += "</table>";

    return table;
}

void handleRoot()
{
    String html = "<!DOCTYPE html><html><head><title>Sauf Tracker</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f4f4f9; color: #333; }";
    html += "h1 { text-align: center; color: #4CAF50; }";
    html += "table { width: 100%; border-collapse: collapse; margin: 20px 0; }";
    html += "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }";
    html += "th { background-color: #4CAF50; color: white; }";
    html += "tr:hover { background-color: #f1f1f1; }";
    html += "</style>";
    html += "</head><body>";
    html += "<h1>Sauf Tracker</h1>";

    html += "<h2>(250-350ml)</h2>";
    html += generateLeaderboard(250, 350);

    html += "<h2>(450-550ml)</h2>";
    html += generateLeaderboard(450, 550);

    html += "<h2>Other</h2>";
    html += generateLeaderboard(0, 1000000);

    html += "</body></html>";

    server.send(200, "text/html", html);
}

void addEntry(const String &name, int time, int quantity)
{
    File file = SPIFFS.open("/leaderboard.txt", FILE_APPEND);
    if (!file)
    {
        Serial.println("Error opening file.");
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
