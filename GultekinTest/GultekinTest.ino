#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "127.0.0.1", 10800);

String ssid = "Galaxy M30s69CA";
String password = "kar.1234";
int WifiValue = 0; // Wifi bağlantı durumu
String YaziYaz;
String YaziNokta;
char c_digit;

int buzzerPin = 21, Termik = 3, TermikValue = 0, TermikTime = 0;
unsigned long bOnTimeMilis = 0;

// INPUT
int bOn = 34, bSet = 32, SetModu = 0, SetModuTime = 0, bMan = 0;
int bOnValue, bOnTime = 0, bArti = 26, bEksi = 25;

// OUTPUT
int RFan = 23;
int RFanValue = 0; // Fan açılışta kapalı
int RPompa = 22;
int RPompaValue = 0; // Sirkülasyon pompası açılışta kapalı
int TaskWebTime = 0;
int TaskWebTimeConnected = 0;

#define PRINT(s) Serial.print(s); // Serial.println(F(s));
#define PRINTA(s)     \
    Serial.println(); \
    Serial.print(s);

void setup()
{
    xTaskCreate(TaskWeb, "GorevWeb", 10000, NULL, 1, NULL);
    // put your setup code here, to run once:
}

void TaskWeb(void *parameter)
{
    (void)parameter;
    delay(4000);
    // UBaseType_t uxHighWaterMark;
    // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    // // PRINTA(""); PRINT("TaskWeb Stack:");  PRINT(uxHighWaterMark);
    // // PRINTA(""); PRINT("TaskWeb CPU: ");   PRINT(xPortGetCoreID());
    for (;;)
    {
        if ((WiFi.status() == WL_CONNECTED))
        {
            if (TaskWebTimeConnected <= (timeClient.getEpochTime() - 10))
            {
                TaskWebTimeConnected = timeClient.getEpochTime();
                PRINT("TaskWeb CPU: ");
            }
        }
        else
        {
            WifiValue = 0;
            if (TaskWebTime <= (timeClient.getEpochTime() - 10))
            {
                TaskWebTime = timeClient.getEpochTime();
                WiFi.begin(ssid, password); // Wifi bağlan
            }
        }
    }
}

void loop()
{
    // put your main code here, to run repeatedly:
}
