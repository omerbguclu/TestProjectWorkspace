
#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#endif
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "secrets.h"

#ifdef THINGSPEAK
#include "ThingSpeak.h"
unsigned long myChannelNumber = CHANNEL_NUMBER;
const char *myWriteAPIKey = API_KEY_WRITE;
#endif

#ifdef DHT_TEMPERATURE
#include <DHT.h>
#include <DHT_U.h>
#define DHTTYPE DHT11 // DHT 11
#define DHTPIN 4      // Digital pin connected to the DHT sensor
DHT_Unified dht(DHTPIN, DHTTYPE);
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
int nextTime = 5 * 60 * 1000; // Do this every second or 1000 milliseconds
unsigned long goTime;
float temperature = 0, humidity = 0;

WiFiClient client;

#ifdef ESP32
WebServer server(80);
#elif defined(ESP8266)
ESP8266WebServer server(80);
#endif

#define DEBUG

void GetTemperatureHumidity()
{
#ifdef DHT_TEMPERATURE
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

    #ifdef DEBUG
    Serial.printf("Temperature: ", temperature);
    Serial.printf("Humidity: ", humidity);
    #endif
#endif
}

void handleRoot()
{
    GetTemperatureHumidity();
    String message = "<!DOCTYPE html><html><body>Temperature:";
    message += temperature;
    message += " Humidity:";
    message += humidity;
#ifdef THINGSPEAK
    message += "<br><iframe width=""450"" height=""260"" style=""border: 1px solid #cccccc;"" src=""https://thingspeak.com/channels/1238834/charts/3?api_key=";
    message += API_KEY_WRITE;
    message += "&bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=400&timescale=10&title=Temperature&type=line""></iframe>";
    message += "<iframe width=""450"" height=""260"" style=""border: 1px solid #cccccc;"" src=""https://thingspeak.com/channels/1238834/charts/4?api_key=";
    message += API_KEY_WRITE;
    message += "&bgcolor=%23ffffff&color=%23d62020&dynamic=false&results=400&timescale=10&title=Humidity&type=line""></iframe>";
#endif
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
    MDNS.begin(DEVICE_HOSTNAME);

    server.on("/", handleRoot);

    server.onNotFound(handleNotFound);

    server.begin();
}

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
#endif
    WiFi.disconnect(true);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(DEVICE_HOSTNAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        // Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("ESP32-C3-01");

#ifdef DEBUG
    Serial.printf("WiFi.status() is %d\n", WiFi.status());
    Serial.printf("client.connected() is %d\n", client.connected());
    Serial.println(WiFi.localIP());
#endif

#ifdef THINGSPEAK
    ThingSpeak.begin(client);
#endif

    ArduinoOTA.begin();

#ifdef DHT_TEMPERATURE
    dht.begin();
#endif

    goTime = millis();

    HandleServer();
}

void functionGo()
{
#ifdef DHT_TEMPERATURE
    GetTemperatureHumidity();
#endif
#ifdef THINGSPEAK
    ThingSpeak.setField(3, temperature);
    ThingSpeak.setField(4, humidity);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
#endif
    goTime = millis() + nextTime;
}

void loop()
{
    ArduinoOTA.handle();
    server.handleClient();
#ifdef ESP8266
    MDNS.update();
#endif
    if (millis() >= goTime)
        functionGo();
}
