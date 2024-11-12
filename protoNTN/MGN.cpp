#include "MGN.h"
#include "AT_commands.h"

MGN::MGN(ATcommands* mod) {
  this->module = mod;
}

MGN::~MGN() {
}

bool MGN::init() {
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.multi_rat_enable\",\"true\"", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.preferred_rat_list\",\"none\"", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.auto_preference_mode\",\"none\"", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%GETACFG=\"ntn.conf.gnss_in_use\"", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%SETACFG=\"modem_apps.Mode.AutoConnectMode\",\"true\"", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%RATIMGSEL=2", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%RATACT=\"NBNTN\",1", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%SETCFG=\"BAND\",\"23\"", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT+CFUN=0", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%NTNEV=\"POSREQ\",1", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%NTNCFG=\"POS\",\"STAT\",\"45.4046\",\"-71.8922\",\"160\"", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%NOTIFYEV=\"SIB31\",1", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%NTNEV=\"TA\",1", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT%NOTIFYEV=\"RRCSTATE\",1", "OK\0", 1000)) return false;
  if (!module->sendCommand("AT+CEREG=2", "OK\0", 1000)) return false;

  return true;
}

bool MGN::switchToLTE(void) {
  if (!module->sendCommand("AT%RATACT=\"CATM\",1", "OK\0", 1000)) return false;
  return true;
}

bool MGN::switchToNTN(void) {
  if (!module->sendCommand("AT%RATACT=\"NBNTN\",1", "OK\0", 1000)) return false;
  return true;
}

bool MGN::estConnecte(void) {
  char reply[255] = { 0 };
  if (!module->sendCommand("AT+CEREG?", "OK\0", reply, 1000)) {
    return false;
  } else {
    // emplacement de '+'
    char* ptr = strstr(reply, "+CEREG:");
    // caracteres 8 et 10 sont x et y dans
    // +CEREG: x,y
    char y = ptr[10];

    if (y == '1' || y == '5') {  // 1 : connected, 5 : connected (roaming)
      return true;
    }
    return false;
  }
}

bool MGN::openSocket(void) {
  // Ouvre le socket 1
  if (!module->sendCommand("AT%SOCKETCMD=\"ALLOCATE\",1,\"UDP\",\"OPEN\",\"137.184.167.242\",11000,11000", "OK\0", 1000)) return false;

  // Met les options necessaires
  if (!module->sendCommand("AT%SOCKETCMD=\"SETOPT\",1,36000,1", "OK\0", 1000)) return false;

  // Active le socket
  if (!module->sendCommand("AT%SOCKETCMD=\"ACTIVATE\",1", "OK\0", 1000)) return false;

  return true;
}

bool MGN::closeSocket(void) {
  // Supprime le socket 1
  if (!module->sendCommand("AT%SOCKETCMD=\"DELETE\",1", "OK\0", 1000)) return false;
}

bool MGN::sendData(void) {
  if (this->openSocket()) {
    if (!module->sendCommand("AT%SOCKETDATA=\"SEND\",1,13,\"4D657373616765646532303042\"", "OK\0", 1000)) {
      return false;
    } else {
      Serial.println("Message envoye.");
    }
    this->closeSocket();
    return true;
  }
  return false;
}

void MGN::initGPS(void) {
}

void MGN::lireGPS(void) {
}

void MGN::updateGPS(void) {
}