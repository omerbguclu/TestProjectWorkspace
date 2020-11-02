#include "src/blink/blink.h"
#include "src/logger/logger.h"
#include "globalVariables.hh"

// define two tasks for Blink & AnalogRead

// the setup function runs once when you press reset or power the board
void setup() {

  // Now set up two tasks to run independently.
  xTaskCreate(
    BlinkThread
    // ,  (const portCHAR *)"Blink"   // A name just for humans
    ,  "BlinkThread"   // A name just for humans
    ,  128  // Stack size
    ,  NULL
    ,  2  // priority
    ,  NULL );

  xTaskCreate(
    LoggerThread
    // ,  (const portCHAR *) "AnalogRead"
    ,  "PrintThread"
    ,  128 // This stack size can be checked & adjusted by reading Highwater
    ,  NULL
    ,  1  // priority
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop(){}