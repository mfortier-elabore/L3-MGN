/* This code is an example of how to use the AT command library to
 *  send AT commands (or ASCII commands) to a module via UART (TX/RX)
 *  using software or hardware serial. The commands can be set up to
 *  check for a specified reply from the module as well as a timeout.
 *  
 *  Author: Timothy Woo (botletics.com)
 *  Last Modified: 10/20/2017
 */

#include "AT_commands.h"
#include "MGN.h"
#include "console.h"
#include "SoftwareSerial.h"

#define RST 4      // MCU pin to control module reset

HardwareSerial *moduleSerial = &Serial1;

ATcommands module = ATcommands(RST, true);  // Use "false" if you don't want AT commands with newline, "true" otherwise

MGN *mgn;
Console * console;

void setup() {

  Serial.begin(115200);

  moduleSerial->begin(115200);  // Verify your module's baud rate/home/marc-fortier/Arduino/libraries/AT-Command-Library-master/ATcommands.cpp
  module.begin(*moduleSerial);

  mgn = new MGN(&module);

  mgn->init();
  
  console = new Console(&module);
}

void loop() {
  //console->loop();
  
  mgn->update();
}
