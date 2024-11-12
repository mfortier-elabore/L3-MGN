#ifndef MGN_H
#define MGN_H

#include "AT_commands.h"

class MGN {
public:
  // membres
  ATcommands* module;

  char GPS_act[32];

  // Socket UDP
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
  bool closeSocket(void);
  bool sendData(void);

  void initGPS(void);
  void lireGPS(void);
  void updateGPS(void);

  void clearBuffer(void);
};


#endif