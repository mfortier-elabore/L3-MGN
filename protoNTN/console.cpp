#include "console.h"

Console::Console(ATcommands *mod)
{
    this->module = mod;
}

Console::~Console(void)
{
}

void Console::loop(void)
{
    Serial.println("Console ProtoNTN demarree, attente de commandes.");

    while (1)
    {
        this->recvWithEndMarker();
        if (this->newData)
        {
            this->newData = false;
            this->asyncCharsReceived = false;

            Serial.print("> ");
            Serial.println(receivedChars);

            char reply[255] = {0};

            if (strstr(receivedChars, "ATZ") != NULL)
            {
                this->module->sendCommand(receivedChars, "BOOT", reply, 10000);
            }
            else
            {

                this->module->sendCommand(receivedChars, "OK", reply, 1000);
            }

            Serial.print("< ");
            Serial.println(reply);
            Serial.println("< ");
        }

        this->asyncCharsReceive();
    }
}

void Console::recvWithEndMarker()
{
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0 && newData == false)
    {
        rc = Serial.read();

        if (rc != endMarker)
        {
            this->receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= NUM_CHARS)
            {
                ndx = NUM_CHARS - 1;
            }
        }
        else
        {
            this->receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void Console::asyncCharsReceive(void)
{
    while (this->module->mySerial->available() > 0)
    {
        char c = this->module->mySerial->read();

        if ((uint8_t)c != 10 && (uint8_t)c != 13)
        {
            if (!this->asyncCharsReceived)
            {
                this->asyncCharsReceived = true;
                Serial.print("\n << ");
            }
        }

        if (this->asyncCharsReceived)
        {
            Serial.print(c);
        }
    }
}
