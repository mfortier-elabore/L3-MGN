#ifndef MGN_H
#define MGN_H

#include "ATcommands.h"

class MGN {
public:
  // membres
  ATcommands* module;

  char GPS_act[32];

  // Socket UDP
  const char SOCKET_ALLOCATE[68] = "AT%SOCKETCMD=\"ALLOCATE\",1,\"UDP\",\"OPEN\",\"137.184.167.242\",11000,11000";
  const char SOCKET_SETOPT[31] = "AT%SOCKETCMD=\"SETOPT\",1,36000,1";
  const char SOCKET_ACTIVATE[25] = "AT%SOCKETCMD=\"ACTIVATE\",1";
  const char SOCKET_SEND[54] = "AT%SOCKETDATA=\"SEND\",1,13,\"48656C6C6F2C20776F726C6421\"";
  const char SOCKET_DELETE[23] = "AT%SOCKETCMD=\"DELETE\",1";
  const char SEND_NTN[41] = "AT%SOCKETDATA=\"SEND\",1,7,\"566961204e544e\"";
  const char SEND_LTE[41] = "AT%SOCKETDATA=\"SEND\",1,7,\"566961204c5445\"";

    // methodes
    MGN(ATcommands * module);
  ~MGN();

  bool init(void);
  bool switchToLTE(void);
  bool switchToNTN(void);

  bool estConnecte(void);
  bool openSocket(void);
  bool sendData(void);

  void initGPS(void);
  void lireGPS(void);
  void updateGPS(void);

  void clearBuffer(void);
};


#endif