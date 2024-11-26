#ifndef MGN_H
#define MGN_H

#include "AT_commands.h"
#include "ledmgr.h"

#define RESEAU_NONE 0
#define RESEAU_LTE 1
#define RESEAU_NTN 2

#define MSG_NUM_BYTES 13

class MGN {
public:
  // membres
  ATcommands* module;

  float latitude;
  float longitude;

  LedManager* led_NTN = new LedManager(12);
  LedManager* led_LTE = new LedManager(11);
  LedManager* led_RX = new LedManager(10);

  uint64_t t_debut;
  const uint64_t TEMPS_BOUCLE = 30 * 1 * 1000;  // Temps en ms pour 10 minutes
  const uint8_t TEMPS_UPDATE = 200;             // 200ms entre les requetes polling
  uint8_t messageEnvoye;
  uint8_t reseauActuel;
  bool connected;

  uint32_t running_minutes;

  uint8_t id;
  uint8_t message[55];

  // Socket UDP
  const char SEND_NTN[41] = "AT%SOCKETDATA=\"SEND\",1,7,\"566961204e544e\"";
  const char SEND_LTE[41] = "AT%SOCKETDATA=\"SEND\",1,7,\"566961204c5445\"";

  // methodes
  MGN(ATcommands* module);
  ~MGN();

  bool init(void);
  bool switchToLTE(void);
  bool switchToNTN(void);

  bool estConnecte(void);
  bool openSocket(void);
  bool closeSocket(void);
  bool sendData(void);

  void prepareMessage(void);

  void initGPS(void);
  void lireGPS(void);
  void updateGPS(void);

  void clearBuffer(void);

  void updateLeds(void);
  void update(void);
};


#endif