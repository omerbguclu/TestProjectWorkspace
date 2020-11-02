bool pinState = false;
unsigned long currentMillis = 0, previousMillis = 0;

bool getPinState(void)
{
    return pinState;
}

void setPinState(bool state)
{
    pinState = state;
}

bool getCurrentMillis(void)
{
    return currentMillis;
}

void setCurrentMillis(bool state)
{
    currentMillis = state;
}

bool getPreviousMillis(void)
{
    return previousMillis;
}

void setPreviousMillis(bool state)
{
    previousMillis = state;
}