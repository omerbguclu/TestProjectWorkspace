#include <SoftwareSerial.h>

#define WIFI_NAME (String)"wifiname"
#define PASSWORD (String)"pwd"

SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

void ReadFromEsp(void)
{
  if(esp8266.available()) // check if the esp is sending a message 
  {
    while(esp8266.available())
    {
      // The esp has data so display its output to the serial window 
      char c = esp8266.read(); // read the next character.
      Serial.write(c);
    }
  }
}

void SendAndReadFromEsp(String message)
{
  esp8266.println(message);
  ReadFromEsp();
  delay(500);
}

void setup()
{
  Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different

  SendAndReadFromEsp("AT");
  SendAndReadFromEsp("AT+GMR");
  SendAndReadFromEsp("AT+CWMODE=1"); /* AP MODE */
  SendAndReadFromEsp("AT+CWJAP=\""+ WIFI_NAME +"\",\"" + PASSWORD + "\"");
  delay(8000);
  SendAndReadFromEsp("AT+CIFSR"); /* FIND IP */
  SendAndReadFromEsp("AT+CIPMUX=1");
  SendAndReadFromEsp("AT+CIPSERVER=1,80"); /* SERVER CREATING */
  SendAndReadFromEsp("AT+CIPSEND=0,37"); /* ID AND LENGTH OF MESSAGE */
  SendAndReadFromEsp("<head> <h1>HELLO WORLD!!</h1> </head>");
}
 
void loop()
{
  if(esp8266.available()) // check if the esp is sending a message 
  {
    while(esp8266.available())
    {
      // The esp has data so display its output to the serial window 
      char c = esp8266.read(); // read the next character.
      Serial.write(c);
    }  
  }
  
  if(Serial.available())
  {
    // the following delay is required because otherwise the arduino will read the first letter of the command but not the rest
    // In other words without the delay if you use AT+RST, for example, the Arduino will read the letter A send it, then read the rest and send it
    // but we want to send everything at the same time.
    delay(1000); 
    
    String command="";
    
    while(Serial.available()) // read the command character by character
    {
        // read one character
      command+=(char)Serial.read();
    }
    esp8266.println(command); // send the read character to the esp8266
  }
}
