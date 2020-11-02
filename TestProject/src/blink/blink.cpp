#include "../../globalVariables.hh"
#include "blink.h"

void BlinkThread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(13, getPinState());   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(ONE_SECOND); // wait for one second
    setPinState(!getPinState());
    digitalWrite(13, getPinState());    // turn the LED off by making the voltage LOW
    vTaskDelay(ONE_SECOND); // wait for one second
  }
}