#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "ThingSpeak.h"
#include <DHT.h>
#include <DHT_U.h>

#ifndef STASSID
#define STASSID "alabamana"
#define STAPSK ".ttnet321,"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
unsigned long myChannelNumber = 1238834;
const char *myWriteAPIKey = "4A2HPLG8ILYXZXZQ";
int nextTime = 5 * 1000; // Do this every second or 1000 milliseconds
unsigned long goTime;

#define DHTTYPE DHT11 // DHT 11
#define DHTPIN 2      // Digital pin connected to the DHT sensor
DHT_Unified dht(DHTPIN, DHTTYPE);

WiFiClient client;

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        // Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname("ESP8266-NODEMCUV2-01");

    Serial.println("PPP");
    Serial.println("PPP");
    Serial.println(WiFi.status());
    Serial.println(client.connected());
    ThingSpeak.begin(client);

    ArduinoOTA.begin();

    goTime = millis();
}

void functionGo()
{
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        Serial.println(F("Error reading temperature!"));
    }
    else
    {
        Serial.print(F("Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        Serial.println(F("Error reading humidity!"));
    }
    else
    {
        Serial.print(F("Humidity: "));
        Serial.print(event.relative_humidity);
        Serial.println(F("%"));
    }

    ThingSpeak.setField(1, 15);
    ThingSpeak.setField(2, 25);
    Serial.println(ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey));
    goTime = millis() + nextTime;
}

void loop()
{
    ArduinoOTA.handle();
    if (millis() >= goTime)
        functionGo();
}
