#ifndef MGN_H
#define MGN_H

#include "AT_commands.h"
#include "ledmgr.h"

#define RESEAU_NONE 0
#define RESEAU_LTE 1
#define RESEAU_NTN 2

#define MSG_NUM_BYTES 13

class MGN
{
public:
  // membres
  ATcommands *module;

  float latitude;
  float longitude;

  LedManager *led_NTN = new LedManager(12);
  LedManager *led_LTE = new LedManager(11);
  LedManager *led_RX = new LedManager(10);

  unsigned long t_debut;
  const unsigned long TEMPS_BOUCLE = 600000L; // Temps en ms pour 10 minutes
  const unsigned long TEMPS_UPDATE = 2000;     // 200ms entre les requetes polling
  uint8_t messageEnvoye;
  uint8_t reseauActuel;
  bool connected;

  uint32_t running_minutes;

  uint8_t id;
  char message[64];

  // methodes
  MGN(ATcommands *module);
  ~MGN();

  bool init(void);
  bool Type1SCInit(void);
  bool switchToLTE(void);
  bool switchToNTN(void);

  bool estConnecte(void);
  bool openSocket_client(void);
  bool openSocket_server(void);
  bool closeSocket(void);
  bool sendData(void);
  bool getData(void);

  void prepareMessage(void);
  void decodeMessage(char * message);
  bool messageRecu(void);

  void attendFixGNSS(void);
  bool lireGNSS(void);

  void updateLeds(void);
  void update(void);
};

#endif