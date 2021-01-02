#include <ESP8266WiFi.h>
#include <DHT.h>

WiFiClient client;
String ssidName = "GIBIRNET_2.4GHz";                     //SSID name
String ssidPwd = ".ttnet321,";                           //SSID password
String apiKey = "4A2HPLG8ILYXZXZQ";                      //Get from thingspeak
String server = "184.106.153.149";                       //Thingspeak ip adresi

#define DHTTYPE DHT11
#define DHTPIN  2
DHT dht(DHTPIN,DHTTYPE);

float humidity = 0, temperature = 0;
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 15000;              // interval at which to read sensor
bool isThereAnyNewData = false;

#define DEBUG_PRINT_ENABLED TRUE
enum{
  START_SERIAL,
  PRINT_CONNECTING,
  PRINT_CONNECTING_DOTS,
  PRINT_CONNECTED,
  PRINT_NEW_DATA,
  PRINT_FAILED_DHT11,
  PRINT_HUMIDITY_AND_TEMPERATURE
};

void setup() {
  WiFi.begin(ssidName, ssidPwd);
  printDebugOutputs(START_SERIAL);
  printDebugOutputs(PRINT_CONNECTING);

  while (WiFi.status() != WL_CONNECTED) {
    printDebugOutputs(PRINT_CONNECTING_DOTS);
    delay(500);
  }
}

void loop() {
  gettemperature();
  if(isThereAnyNewData == true)
  {
    isThereAnyNewData = false;
    printDebugOutputs(PRINT_NEW_DATA);
    if (client.connect(server,80)) // "184.106.153.149" or api.thingspeak.com
    {
      String postStr = "GET https://api.thingspeak.com/update?api_key=";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıcaz.                                  
      postStr += apiKey;
      postStr += "&field1=";
      postStr += String(temperature);
      postStr += "&field2=";
      postStr += String(humidity);
      postStr += "\r\n\r\n";
      delay(1000);

      client.print(postStr + " HTTP/1.1\r\n" +
       "Host: api.thingspeak.com\r\n" +
       "Connection: close\r\n\r\n");
    }
  }
}

void gettemperature() {
  /* Wait at least 2 seconds seconds between measurements. if the difference 
     between the current time and last time you read the sensor is bigger than
     the interval you set, read the sensor Works better than delay for things 
     happening elsewhere also */
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    /* save the last time you read the sensor */
    previousMillis = currentMillis;

    /* Reading temperature for humidity takes about 250 milliseconds! Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor) */
    humidity = dht.readHumidity();          // Read humidity (percent)
    temperature = dht.readTemperature();    // Read temperature as Fahrenheit
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      printDebugOutputs(PRINT_FAILED_DHT11);
      return;
    }
    else{
      printDebugOutputs(PRINT_HUMIDITY_AND_TEMPERATURE);
    }

    isThereAnyNewData = true;
  }
}

void printDebugOutputs(unsigned char state){
  #if(DEBUG_PRINT_ENABLED == TRUE)
  if(state == START_SERIAL)
  {
    Serial.begin(9600);
  }
  else if (state == PRINT_CONNECTING)
  {
    Serial.println("Connecting");
  }
  else if (state == PRINT_CONNECTING_DOTS)
  {
    Serial.print(".");
  }
  else if (state == PRINT_CONNECTED)
  {
    Serial.println();
    Serial.println("Connected");
  }
  else if (state == PRINT_NEW_DATA)
  {
    Serial.println("New Data...");
  }
  else if (state == PRINT_FAILED_DHT11)
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else if (state == PRINT_HUMIDITY_AND_TEMPERATURE)
  {
    Serial.println("Humidity ->" + String(humidity));
    Serial.println("Temperature ->" + String(temperature));
  }
  #endif
}
