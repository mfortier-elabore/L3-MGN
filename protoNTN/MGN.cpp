#include "MGN.h"
#include "AT_commands.h"
#include "runningHours.h"

MGN::MGN(ATcommands *mod)
{
  this->module = mod;
}

MGN::~MGN()
{
}

bool MGN::init()
{
  // leds
  led_NTN = new LedManager(12);
  led_LTE = new LedManager(11);
  led_RX = new LedManager(10);

  // Temps
  this->t_debut = 0;
  MGN_RunningHours_init();

  // Machine a etats
  this->messageEnvoye = 0;
  this->reseauActuel = RESEAU_NONE;
  this->connected = false;
  this->id = 5;

  this->running_minutes = 2833; // get_running_minutes(); // récuperer depuis EEPROM

  this->latitude = 45.403975;
  this->longitude = -71.891657;

  bool err = false;

  // Config module
  module->sendCommand("ATZ", "%BOOTEV:0\0", 25000);
  delay(5000);
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.multi_rat_enable\",\"true\"", "OK\0", 10000))
    err = false;
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.preferred_rat_list\",\"none\"", "OK\0", 10000))
    err = false;
  if (!module->sendCommand("AT%SETACFG=\"radiom.config.auto_preference_mode\",\"none\"", "OK\0", 10000))
    err = false;
  if (!module->sendCommand("AT%SETACFG=\"locsrv.operation.locsrv_enable\",\"true\"", "OK\0", 10000)) //+
    err = false;
  if (!module->sendCommand("AT%SETACFG=\"locsrv.internal_gnss.auto_restart\",\"enable\"", "OK\0", 10000))
    err = false;
  // if (!module->sendCommand("AT%GETACFG=\"ntn.conf.gnss_in_use\"", "OK\0", 10000)) //-
  //   err = false;
  if (!module->sendCommand("AT%SETACFG=\"modem_apps.Mode.AutoConnectMode\",\"true\"", "OK\0", 10000))
    err = false;
  module->sendCommand("ATZ", "%BOOTEV:0\0", 25000); //+
  delay(5000);
  if (!module->sendCommand("AT%RATACT=\"NBNTN\",\"1\"", "OK\0", 10000)) //+
    err = false;
  if (!module->sendCommand("AT%RATIMGSEL=2", "OK\0", 10000)) //+
    err = false;
  if (!module->sendCommand("AT+CFUN=0", "OK\0", 10000)) //+
    err = false;
  if (!module->sendCommand("AT%SETCFG=\"BAND\",\"23\"", "OK\0", 10000))
    err = false;
  //  if (!module->sendCommand("AT+CFUN=0", "OK\0", 10000))  //-
  //    err = false;
  //  if (!module->sendCommand("AT%NTNEV=\"POSREQ\",1", "OK\0", 10000)) //-
  //    err = false;
  //  if (!module->sendCommand("AT%NTNCFG=\"POS\",\"STAT\",\"45.4046\",\"-71.8922\",\"160\"", "OK\0", 10000)) //-
  //    err = false;

  if (!module->sendCommand("AT%NTNCFG=\"POS\",\"IGNSS\",\"1\"", "OK\0", 10000))
    err = false;
  if (!module->sendCommand("AT%IGNSSEV=\"FIX\",1", "OK\0", 10000))
    err = false;
  if (!module->sendCommand("AT%NOTIFYEV=\"SIB31\",1", "OK\0", 10000))
    err = false;
  //  if (!module->sendCommand("AT%NTNEV=\"TA\",1", "OK\0", 10000)) //-
  //    err = false;
  //  if (!module->sendCommand("AT%NOTIFYEV=\"RRCSTATE\",1", "OK\0", 10000)) //-
  //    err = false;
  if (!module->sendCommand("AT+CEREG=2", "OK\0", 10000))
    err = false;
  if (!module->sendCommand("AT%IGNSSACT=1", 10000))
    err = false;

  char reply[255] = {0};

  do
  {
    module->sendCommand("AT%IGNSSINFO=\"LASTFIX\"", "OK\0", reply, 10000);
    Serial.println(reply);

    module->sendCommand("AT%IGNSSACT?", "OK\0", reply, 10000);
    Serial.println(reply);

    if (strstr(reply, "%IGNSSACT: 0") != NULL)
    {
      Serial.print("Activation INGSS .. ");
      module->sendCommand("AT%IGNSSACT=1", "OK\0", 10000);
    }

    delay(2000);
    Serial.print(".");
  } while (1);

  if (!module->sendCommand("AT+CFUN=1", 10000))
    err = false;

  if (!switchToLTE())
    err = false;

  if (err)
    Serial.println("INITIALISATION A ECHOUE.");

  return err;
}

bool MGN::switchToLTE(void)
{
  if (this->reseauActuel == RESEAU_LTE)
  {
    return true;
  }

  Serial.println("Changement reseau vers LTE.");
  this->led_NTN->eteint();

  if (!module->sendCommand("AT%RATACT=\"CATM\",1", "OK\0", 10000))
  {
    return false;
  }

  this->led_LTE->setEtat(1);
  this->led_LTE->setFlashe(1);
  this->led_LTE->allume();
  this->led_NTN->setEtat(0);

  this->reseauActuel = RESEAU_LTE;
  Serial.println("Reseau LTE.");

  return true;
}

bool MGN::switchToNTN(void)
{

  if (this->reseauActuel == RESEAU_NTN)
  {
    return true;
  }

  Serial.println("Changement reseau vers NTN.");
  this->led_LTE->eteint();

  if (!module->sendCommand("AT%RATACT=\"NBNTN\",1", "OK\0", 10000))
  {
    return false;
  }

  this->led_NTN->setEtat(1);
  this->led_NTN->setFlashe(1);
  this->led_NTN->allume();
  this->led_LTE->setEtat(0);

  this->reseauActuel = RESEAU_NTN;

  Serial.println("Reseau NTN.");

  return true;
}

bool MGN::estConnecte(void)
{
  char reply[255] = {0};
  if (!module->sendCommand("AT+CEREG?", "OK\0", reply, 1000))
  {
    // Commande a echoue, mais pas necessaire deconnecte
    return false;
  }
  else
  {
    // emplacement de '+'
    char *ptr = strstr(reply, "+CEREG:");
    // caracteres 8 et 10 sont x et y dans
    // +CEREG: x,y
    char y = ptr[10];

    if (y == '1' || y == '5')
    { // 1 : connected, 5 : connected (roaming)
      this->connected = true;
      // Leds solides = connecté
      if (this->reseauActuel == RESEAU_NTN)
      {
        this->led_NTN->setFlashe(0);
      }
      else
      {
        this->led_LTE->setFlashe(0);
      }
      return true;
    }

    // Leds flashe etat connexion en cours
    if (this->reseauActuel == RESEAU_NTN)
    {
      this->led_NTN->setFlashe(1);
    }
    else
    {
      this->led_LTE->setFlashe(1);
    }
    this->connected = false;
    return false;
  }
}

bool MGN::openSocket(void)
{
  // Ouvre le socket 1
  if (!module->sendCommand("AT%SOCKETCMD=\"ALLOCATE\",1,\"UDP\",\"OPEN\",\"137.184.167.242\",11000,11000", "OK\0", 10000))
    return false;

  // Met les options necessaires
  if (!module->sendCommand("AT%SOCKETCMD=\"SETOPT\",1,36000,1", "OK\0", 10000))
    return false;

  // Active le socket
  if (!module->sendCommand("AT%SOCKETCMD=\"ACTIVATE\",1", "OK\0", 10000))
    return false;

  return true;
}

bool MGN::closeSocket(void)
{
  // Supprime le socket 1
  Serial.println("Socket ferme. ");
  if (!module->sendCommand("AT%SOCKETCMD=\"DELETE\",1", "OK\0", 10000))
    return false;
}

bool MGN::sendData(void)
{

  // char msg[54] = "AT%SOCKETDATA=\"SEND\",1,13,\"4D657373616765646532303042\"";
  this->prepareMessage();
  Serial.print("Message a envoyer : ");
  Serial.println((char *)this->message);

  if (this->openSocket())
  {
    Serial.print("Socket ouvert. ");
    if (!module->sendCommand(this->message, "OK\0", 10000))
    {
      this->closeSocket();
      return false;
    }
    else
    {
      Serial.println("Message envoye.");
    }
    this->closeSocket();
    return true;
  }
  return false;
}

void MGN::initGPS(void)
{
  module->sendCommand("AT%SETACFG=\"locsrv.operation.locsrv_enable\",\"true\"", "OK\0", 10000);
  module->sendCommand("AT%SETACFG=\"locsrv.internal_gnss.auto_restart\",\"enable\"", "OK\0", 10000);
  module->sendCommand("ATZ", "%BOOTEV:0\0", 25000);
  delay(5000);
  module->sendCommand("AT%NTNCFG=\"POS\",\"IGNSS\",\"1\"", "OK\0", 10000);
  module->sendCommand("AT%IGNSSACT=1", "OK\0", 10000);
}

void MGN::lireGPS(void)
{
  char pos[255] = {0};

  module->sendCommand("AT%IGNSSINFO=\"LASTFIX\"", "OK\0", pos, 1000);
  Serial.println(pos);

  // Réponse si pas de position GPS
  // AT%IGNSSINFO="LASTFIX"
  //
  //%IGNSSINFO: 0
  //
  // OKK
  if (strstr(pos, "%IGNSSINFO: 0") != NULL)
  {
    Serial.println("Position GPS inconnue.");
    this->latitude = 0;
    this->longitude = 0;
  }

  // Reponse si position GPS:
  // %IGNSSINFO: 2,"21:29:14","12/11/2024","45.403975","-71.891657","40.0",1731446954000,,,"B",3
  // OK
  // char pos[100] = "%IGNSSINFO: 2,\"21:29:14\",\"12/11/2024\",\"45.403975\",\"-71.891657\",\"40.0\",1731446954000,,,\"B\",3\x0\xd\x0\xdOK";
  char *ptr = strstr(pos, "%IGNSSINFO:");

  if (ptr == NULL)
  {
    return;
  }

  char *ptr_latitude = ptr + 39;  // longueur 9
  char *ptr_longitude = ptr + 51; // longueur 10 (a cause du -)

  char char_latitude[11] = "";
  char char_longitude[11] = "";

  uint8_t n = 9; /*
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

  uint8_t message[13] = { 0 };

  message[0]

  */
}

void MGN::updateGPS(void)
{
}

void MGN::updateLeds(void)
{
  this->led_NTN->update();
  this->led_LTE->update();
  this->led_RX->update();
}

void MGN::prepareMessage()
{
  // Message a envoyer
  uint8_t network;
  uint8_t id = this->id;
  uint8_t cmd = 0;
  float lat = this->latitude;
  float longitude = this->longitude;
  uint32_t running_minutes = this->running_minutes;
  uint8_t checksum = 0;

  uint8_t payload[MSG_NUM_BYTES] = {0};

  // Valeurs
  if (this->reseauActuel == RESEAU_NTN)
  {
    network = 1;
  }
  else
  {
    network = 0;
  }

  // Byte 0
  this->message[0] = id | (network << 5) | (cmd << 6);

  // Bytes 1:4
  float *lat_p = &lat;
  uint8_t *p = (uint8_t *)lat_p;

  for (int i = 0; i < 4; ++i)
  {
    payload[i + 1] = *(p + i);
  }

  // Bytes 5:9
  float *long_p = &longitude;
  p = (uint8_t *)long_p;

  for (int i = 0; i < 4; ++i)
  {
    payload[i + 5] = *(p + i);
  }

  // Bytes 9:12
  int32_t *runm_p = &running_minutes;
  p = (uint8_t *)runm_p;

  for (int i = 0; i < 3; ++i)
  {
    payload[i + 9] = *(p + i);
  }

  // Byte 13
  Serial.print("\nCalcul checksum : ");
  for (int i = 0; i < 12; ++i)
  {
    checksum += payload[i];
    Serial.print(checksum);
    Serial.print(" ");
  }
  Serial.println();

  payload[12] = checksum;

  // Resultat
  Serial.print("\nPayload : ");
  for (int i = 0; i < 12; ++i)
  {
    Serial.print(payload[i]);
    Serial.print(" ");
  }
  Serial.println();

  // buffer
  memset(this->message, 0, 63);

  Serial.print("1");

  // uint8_t data[13] = { 0xa1, 0x00, 0x0b, 0x11, 0xc2, 0x8f, 0xc8, 0x87, 0x42, 0x35, 0x9d, 0xac, 0x25 };
  uint8_t index = 0;

  index += sprintf(this->message, "AT%%SOCKETDATA");

  Serial.print("a");

  index += sprintf(this->message + index, "=\"SEND\",1,13,\"");

  Serial.print("2");

  // Message a l'envers pour coincider avec prog sur le serveur
  for (int8_t i = 0; i < 13; ++i)
  {
    index += sprintf(this->message + index, "%02X", payload[i]);
    Serial.print(".");
  }

  Serial.print("3");

  sprintf(this->message + index, "\"");

  Serial.print("4");

  Serial.print("Message : ");
  Serial.println((char *)this->message);
}

void MGN::update(void)
{
  static unsigned long dernier_update = 0;
  updateLeds();
  MGN_RunningHours_update();
  delay(10);

  // Delai entre les requetes
  if (dernier_update + TEMPS_UPDATE < millis())
  {

    dernier_update = millis();

    // Boucle de 10 minutes écoulée ?
    if (this->t_debut + this->TEMPS_BOUCLE < millis())
    {
      Serial.println("Boucle terminee, nouvelle boucle.");
      this->messageEnvoye = 0;
      this->t_debut = millis();
      uint8_t i = 0;
      do
      {
        delay(10);
        ++i;
      } while (!this->switchToLTE() && i < 5);
    }
    else
    {
      /*Serial.print("Nope : ");
      Serial.print(this->t_debut);
      Serial.print(" + ");
      Serial.print(this->TEMPS_BOUCLE);
      Serial.print(" > ");
      Serial.println(millis());*/
      Serial.print(".");
    }

    // Message envoyé ?
    if (!this->messageEnvoye)
    {
      if (this->estConnecte())
      {
        if (this->sendData())
        {
          this->messageEnvoye = 1;
        }
        else
        {
          // Echec envoi du message
        }
      }
    }
    else
    {
      // TODO: Mode ecoute
    }

    // Moitié du temps écoulé dans la boucle de 10 min
    if (!this->messageEnvoye && this->t_debut + this->TEMPS_BOUCLE / 2 < millis())
    {
      uint8_t i = 0;
      do
      {
        delay(10);
        ++i;
      } while (!this->switchToNTN() && i < 5);
    }
  }
}