/*
 * ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Publish Example
 * Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
 * Made as part of my MQTT Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
 */
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

const int ledPin = 0; // This code uses the built-in led for visual feedback that the button has been pressed
const int buttonPin = 13; // Connect your button to pin #13

// WiFi
// Make sure to update this for your own WiFi network!
const char* ssid = "GIBIRNET_2.4GHz";
const char* wifi_password = ".ttnet321,";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "192.168.1.57";
const char* mqtt_topic = "/test/speak";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "ESP8266NODEMCUV3-1";
int counter = 0;

// Initialise the Pushbutton Bouncer object

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

void setup() {
  pinMode(ledPin, OUTPUT);

  // Switch the on-board LED off to start with
  digitalWrite(ledPin, HIGH);


  // Begin Serial on 115200
  // Remember to choose the correct Baudrate on the Serial monitor!
  // This is just for debugging purposes
  Serial.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  while (!client.connect(clientID)) {
    yield();
    Serial.println("Connection to MQTT Broker failed...");
    delay(5000);
  }
  
  Serial.println("Connected to MQTT Broker!");
}

void loop() {
    String as = "Button pressed! ";
    as += counter++;
    char *ac = new char[as.length() + 1];
    strcpy(ac, as.c_str());
    if (client.publish(mqtt_topic, ac)) {
      Serial.println("Button pushed and message sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(mqtt_topic, "Button pressed!");
    }
    delay(1000);
}
