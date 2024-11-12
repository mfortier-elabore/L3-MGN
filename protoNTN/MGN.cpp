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

  char msg[54] = "AT%SOCKETDATA=\"SEND\",1,13,\"4D657373616765646532303042\"";

  sprintf(msg, "AT%SOCKETDATA=\"SEND\",1,%i,\"4D657373616765646532303042\"", n, );

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
  // Reponse attendue :
  // %IGNSSINFO: 2,"21:29:14","12/11/2024","45.403975","-71.891657","40.0",1731446954000,,,"B",3
  // OK
  char pos[100] = "%IGNSSINFO: 2,\"21:29:14\",\"12/11/2024\",\"45.403975\",\"-71.891657\",\"40.0\",1731446954000,,,\"B\",3\x0\xd\x0\xdOK";
  char* ptr = strstr(pos, "%IGNSSINFO:");

  char * ptr_latitude = ptr + 39; // longueur 9
  char * ptr_longitude = ptr +51; // longueur 10 (a cause du -)

  char char_latitude[11] = "";
  char char_longitude[11] = "";
  
  uint8_t n = 9;
  if(ptr_latitude[0] == 0x2D) {
    n = 10;
  }

  for(uint8_t i=0; i<n; ++i) {
    char_latitude[i] = *(ptr_latitude+i);
    char_longitude[i] = *(ptr_longitude+i);
  }

  n = 9;
  if(ptr_longitude[0] == 0x2D) {
    n = 10;
    Serial.println("moins");
  } else {
    Serial.println(ptr_longitude[0]);
  }

  for(uint8_t i=0; i<n; ++i) {
    longitude[i] = *(ptr_longitude+i);
  }

  this->latitude = atof(char_latitude);
  this->longitude = atof(char_longitude);
}

void MGN::updateGPS(void) {
}