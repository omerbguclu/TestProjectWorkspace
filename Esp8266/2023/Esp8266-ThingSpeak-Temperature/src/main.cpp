#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "ThingSpeak.h"
#include "secrets.h"
#include <DHT.h>
#include <DHT_U.h>

const char *ssid = STASSID;
const char *password = STAPSK;
unsigned long myChannelNumber = CHANNEL_NUMBER;
const char *myWriteAPIKey = API_KEY_WRITE;
int nextTime = 5 * 60 * 1000; // Do this every second or 1000 milliseconds
unsigned long goTime;
float temperature = 0, humidity = 0;

#define DHTTYPE DHT11 // DHT 11
#define DHTPIN 2      // Digital pin connected to the DHT sensor
DHT_Unified dht(DHTPIN, DHTTYPE);

WiFiClient client;

ESP8266WebServer server(80);

// #define DEBUG

void GetTemperatureHumidity()
{
    sensors_event_t event;

    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        goTime = millis() + nextTime;
        return;
    }

    temperature = event.temperature;

    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        goTime = millis() + nextTime;
        return;
    }
    humidity = event.relative_humidity;
}

void handleRoot()
{
    GetTemperatureHumidity();
    String message = "<!DOCTYPE html><html><body>Temperature:";
    message += temperature;
    message += " Humidity:";
    message += humidity;
    message += "<br><iframe width=""450"" height=""260"" style=""border: 1px solid #cccccc;"" src=""https://thingspeak.com/channels/1238834/charts/1?api_key=";
    message += API_KEY_WRITE;
    message += "&bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=400&timescale=10&title=Temperature&type=line""></iframe>";
    message += "<iframe width=""450"" height=""260"" style=""border: 1px solid #cccccc;"" src=""https://thingspeak.com/channels/1238834/charts/2?api_key=";
    message += API_KEY_WRITE;
    message += "&bgcolor=%23ffffff&color=%23d62020&dynamic=false&results=400&timescale=10&title=Humidity&type=line""></iframe>";
    message += "</body></html>";
    server.send(200, "text/html", message);
}

void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void HandleServer()
{
    MDNS.begin("esp8266");

    server.on("/", handleRoot);

    server.onNotFound(handleNotFound);

    server.begin();
}

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
#endif

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        // Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname("ESP8266-01");

#ifdef DEBUG
    Serial.println(WiFi.status());
    Serial.println(client.connected());
#endif

    ThingSpeak.begin(client);

    ArduinoOTA.begin();

    dht.begin();

    goTime = millis();

    HandleServer();
}

void functionGo()
{
    GetTemperatureHumidity();
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    goTime = millis() + nextTime;
}

void loop()
{
    ArduinoOTA.handle();
    server.handleClient();
    MDNS.update();
    if (millis() >= goTime)
        functionGo();
}
