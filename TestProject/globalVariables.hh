#include <Arduino_FreeRTOS.h>
#include <Arduino.h>

#define ONE_SECOND ((1000 / portTICK_PERIOD_MS))
#define ONE_MILLISECOND (1 / portTICK_PERIOD_MS)

bool getPinState(void);
void setPinState(bool state);
bool getCurrentMillis(void);
void setCurrentMillis(bool state);
bool getPreviousMillis(void);
void setPreviousMillis(bool state);