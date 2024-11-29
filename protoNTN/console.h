#ifndef CONSOLE_H
#define CONSOLE_H

#include <Arduino.h>
#include "AT_commands.h"

#define NUM_CHARS 255

class Console
{
public:
    Console(ATcommands *);
    ~Console(void);

    void loop(void);

private:
    bool newData = false;
    bool asyncCharsReceived = false;

    char receivedChars[NUM_CHARS]; // an array to store the received data
    ATcommands *module;

    void recvWithEndMarker(void);
    void asyncCharsReceive(void);
};

#endif // CONSOLE_H