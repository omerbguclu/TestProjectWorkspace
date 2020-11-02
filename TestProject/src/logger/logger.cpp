#include "../../globalVariables.hh"
#include "logger.h"

unsigned int abc = -1;

void LoggerThread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  String metin = "<head> <h1>Merhaba Dunya</h1> </head>";
  for (;;)
  {
    setCurrentMillis(millis());
    Serial.println(metin.length());
    Serial.println(getPinState());
    Serial.println(getCurrentMillis() - getPreviousMillis());
    vTaskDelay(ONE_SECOND);
    setPreviousMillis(getCurrentMillis());
  }
}