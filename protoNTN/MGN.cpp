#include "MGN.h"
#include "ATcommands.h"

MGN::MGN(ATcommands* mod) {
  this->module = mod;
}

MGN::~MGN() {
}

bool MGN::init() {
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.multi_rat_enable\",\"true\"", "OK", 1000)) return false;
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.preferred_rat_list\",\"none\"", "OK", 1000)) return false;
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.auto_preference_mode\",\"none\"", "OK", 1000)) return false;
  if (!module->sendCommand("AT%GETACFG=\"ntn.conf.gnss_in_use\"", "OK", 1000)) return false;
  if (!module->sendCommand("AT%SETACFG=\"modem_apps.Mode.AutoConnectMode\",\"true\"", "OK", 1000)) return false;
  if (!module->sendCommand("AT%RATIMGSEL=2", "OK", 1000)) return false;
  if (!module->sendCommand("AT%RATACT=\"NBNTN\",1", "OK", 1000)) return false;
  if (!module->sendCommand("AT%SETCFG=\"BAND\",\"23\"", "OK", 1000)) return false;
  if (!module->sendCommand("AT+CFUN=0", "OK", 1000)) return false;
  if (!module->sendCommand("AT%NTNEV=\"POSREQ\",1", "OK", 1000)) return false;
  if (!module->sendCommand("AT%NTNCFG=\"POS\",\"STAT\",\"45.4046\",\"-71.8922\",\"160\"", "OK", 1000)) return false;
  if (!module->sendCommand("AT%NOTIFYEV=\"SIB31\",1", "OK", 1000)) return false;
  if (!module->sendCommand("AT%NTNEV=\"TA\",1", "OK", 1000)) return false;
  if (!module->sendCommand("AT%NOTIFYEV=\"RRCSTATE\",1", "OK", 1000)) return false;
  if (!module->sendCommand("AT+CEREG=2", "OK", 1000)) return false;

  return true;
}

void MGN::clearBuffer() {
  for (uint8_t i = 0; this->module->replybuffer[i] != NULL; ++i) {
    this->module->replybuffer[i] = "";
  }
}

bool MGN::switchToLTE(void) {
  if (!module->sendCommand("AT%RATACT=\"CATM\",1", "OK", 1000)) return false;
  return true;
}

bool MGN::switchToNTN(void) {
  if (!module->sendCommand("AT%RATACT=\"NBNTN\",1", "OK", 1000)) return false;
  return true;
}

bool MGN::estConnecte(void) {
  this->clearBuffer();
  if (!module->sendCommand("AT+CEREG?", "+CEREG: 0,1", 1000)) return false;
  return false;
}

bool MGN::openSocket(void) {
  return false;
}
bool MGN::sendData(void) {
  return false;
}

void MGN::initGPS(void) {
}

void MGN::lireGPS(void) {
}

void MGN::updateGPS(void) {
}