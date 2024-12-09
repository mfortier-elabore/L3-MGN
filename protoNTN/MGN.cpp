#include "MGN.h"
#include "AT_commands.h"
#include "runningHours.h"

MGN::MGN(ATcommands *mod)
{
  this->module = mod;
  this->commande = false;
  this->getID();
}

MGN::~MGN()
{
}

bool MGN::Type1SCInit()
{
  char reply[255] = {0};
  bool err = false;

  Serial.println("Initialisation du module type1SC ... ");
  Serial.print("Attente demarrage ");

  // Config module
  do
  {
    Serial.print(".");
  } while (!module->sendCommand("AT", "OK", reply, 20000));
  Serial.println(" OK!");

  Serial.print("Configuration ...");

  do
  {
    Serial.print(".");
    err = false;

    if (!module->sendCommand("ATZ", "BOOTEV:0", reply, 20000))
      err = true;

    if (!module->sendCommand("AT%SETACFG=\"radiom.config.multi_rat_enable\",\"true\"", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%SETACFG=\"radiom.config.preferred_rat_list\",\"none\"", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%SETACFG=\"radiom.config.auto_preference_mode\",\"none\"", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%SETACFG=\"locsrv.operation.locsrv_enable\",\"true\"", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%SETACFG=\"locsrv.internal_gnss.auto_restart\",\"enable\"", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%SETACFG=\"modem_apps.Mode.AutoConnectMode\",\"true\"", "OK\0", reply, 10000))
      err = true;

    if (!module->sendCommand("ATZ", "BOOTEV:0", reply, 20000))
      err = true;

    if (!module->sendCommand("AT%RATACT=\"NBNTN\",\"1\"", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT+CFUN=0", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%PDNSET=1,\"DATA.MONO\",\"IP\"", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%SETSYSCFG=SW_CFG.nb_band_table.band#1,ENABLE;23", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%NTNCFG=\"POS\",\"IGNSS\",\"1\"", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT+CFUN=0", "OK\0", reply, 10000))
      err = true;
    if (!module->sendCommand("AT%IGNSSACT=1", "OK\0", reply, 10000))
      err = true;
    /*    if (!module->sendCommand("AT%GETACFG=\"ntn.conf.gnss_in_use\"", "OK\0", reply, 10000))
          err = true;*/

  } while (err);
  Serial.println(" OK!");

  return true;
}

bool MGN::init()
{
  // Bouton
  pinMode(BOUTON_PIN, INPUT_PULLUP);

  // leds
  led_NTN = new LedManager(11);
  led_LTE = new LedManager(12);
  led_RX = new LedManager(10);

  // Temps
  this->t_debut = 0;
  MGN_RunningHours_init();

  // Machine a etats
  this->messageEnvoye = 0;
  this->reseauActuel = RESEAU_NONE;
  this->connected = false;

  this->running_minutes = MGN_RunningHours_getDigitalInCount();

  this->latitude = 0;
  this->longitude = 0;

  bool err = false;

  char reply[255] = {0};

  this->Type1SCInit();

  this->attendFixGNSS();
  do
  {
  } while (!this->lireGNSS());

  // On rallume la radio
  do
  {
  } while (!module->sendCommand("AT+CFUN=1", "OK\0", reply, 10000));

  // Reseau par defaut = LTE
  if (!switchToLTE())
    err = true;

  if (err)
    Serial.println("INITIALISATION A ECHOUE.");

  return err;
}

bool MGN::switchToLTE(void)
{
  char reply[255] = {0};

  this->led_LTE->setEtat(1);
  this->led_LTE->setFlashe(1);
  this->led_LTE->allume();
  this->led_NTN->setEtat(0);

  if (this->reseauActuel == RESEAU_LTE)
  {
    return true;
  }

  Serial.println("Changement reseau vers LTE.");
  
  do
  {
  } while (!module->sendCommand("AT%RATACT=\"CATM\",1", "OK\0", reply, 10000));


  this->reseauActuel = RESEAU_LTE;
  Serial.println("Reseau LTE.");

  return true;
}

bool MGN::switchToNTN(void)
{
  char reply[255] = {0};

  this->led_NTN->setEtat(1);
  this->led_NTN->setFlashe(1);
  this->led_NTN->allume();
  this->led_LTE->setEtat(0);

  if (this->reseauActuel == RESEAU_NTN)
  {
    return true;
  }

  Serial.println("Changement reseau vers NTN.");

  do
  {
  } while (!module->sendCommand("AT%RATACT=\"NBNTN\",1", "OK\0", reply, 10000));

  this->reseauActuel = RESEAU_NTN;

  Serial.println("Reseau NTN.");

  return true;
}

bool MGN::estConnecte(void)
{
  char reply[255] = {0};
  if (!module->sendCommand("AT+CEREG?", "OK\0", reply, 5000))
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
      if (!this->connected)
      {
        this->connected = true;
        // Leds solides = connecté
        if (this->reseauActuel == RESEAU_NTN)
        {
          Serial.println("Connecte au reseau NTN.");
          this->led_NTN->setFlashe(0);
        }
        else
        {
          Serial.println("Connecte au reseau LTE.");
          this->led_LTE->setFlashe(0);
        }
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
    if (this->connected)
    {
      Serial.println("Deconnecte.");
      this->connected = false;
    }
    return false;
  }
}

bool MGN::openSocket_server(void)
{
  // Verifie si le socket est deja ouvert
  // AT%SOCKETCMD?
  // AT%SOCKETCMD?
  //
  // OK
  char reply[255] = {0};

  if (!module->sendCommand("AT%SOCKETCMD?", "OK\0", reply, 10000))
    return false;

  if (strstr(reply, "%SOCKETCMD:1,\"ACTIVATED\"") != NULL)
  {
    // Serial.println("Socket deja ouvert.");
    return true;
  }

  // Ouvre le socket 1
  if (!module->sendCommand("AT%SOCKETCMD=\"ALLOCATE\",1,\"UDP\",\"LISTEN\",,,7,,,1", "OK\0", reply, 10000))
    return false;

  // %SOCKETCMD:1,"DEACTIVATED"
  //
  // OK

  // Active le socket
  if (!module->sendCommand("AT%SOCKETCMD=\"ACTIVATE\",1", "OK\0", reply, 10000))
    return false;

  Serial.println("Socket ouvert.");

  return true;
}

bool MGN::openSocket_client(void)
{
  char reply[255] = {0};

  // Ouvre le socket 1
  if (!module->sendCommand("AT%SOCKETCMD=\"ALLOCATE\",1,\"UDP\",\"OPEN\",\"137.184.167.242\",11000,11000", "OK\0", reply, 10000))
    return false;

  // Met les options necessaires
  if (!module->sendCommand("AT%SOCKETCMD=\"SETOPT\",1,36000,1", "OK\0", reply, 10000))
    return false;

  // Active le socket
  if (!module->sendCommand("AT%SOCKETCMD=\"ACTIVATE\",1", "OK\0", reply, 10000))
    return false;

  return true;
}

bool MGN::closeSocket(void)
{
  char reply[255] = {0};

  // Supprime le socket 1
  Serial.println("Socket ferme. ");
  if (!module->sendCommand("AT%SOCKETCMD=\"DELETE\",1", "OK\0", reply, 10000))
  {
    Serial.println("Erreur! ");
    return false;
  }
}

bool MGN::sendData(void)
{
  this->prepareMessage();
  Serial.print("Message a envoyer : ");
  Serial.println((char *)this->message);

  if (this->openSocket_client())
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

bool MGN::getData(void)
{
  char reply[255] = {0};

  if (!this->openSocket_server())
  {
    return false;
  }

  if (!module->sendCommand("AT%SOCKETDATA=\"RECEIVE\",1,1500", "OK\0", reply, 10000))
  {
    Serial.println("Commande Receive a echoue.");
    return false;
  }

  if (strstr(reply, "%SOCKETDATA:1") != NULL)
  {
    return this->messageRecu(reply);
  }

  Serial.println("Reply etait invalide.");
  return false;
}

void MGN::getID(void)
{
  byte buf = EEPROM.read(MEMORY_ADDRESS_ID);
  this->id = (uint8_t)buf;

  Serial.print("ID : ");
  Serial.println(this->id);
}

void MGN::setID(uint8_t id)
{
  byte buf = (byte)id;
  EEPROM.write(MEMORY_ADDRESS_ID, buf);

  this->getID();
}

void MGN::attendFixGNSS(void)
{
  char reply[255] = {0};

  Serial.print("Recherche position GNSS...");
  this->led_RX->setEtat(1);
  this->led_RX->setFlashe(1);
  this->led_NTN->setEtat(0);
  this->led_LTE->setEtat(0);
  uint8_t enter = 0;

  module->sendCommand("AT+CFUN=0", "OK\0", reply, 10000);

  do
  {
    module->sendCommand("AT%IGNSSINFO=\"FIX\"", "OK\0", reply, 10000);
    if (strstr(reply, "%IGNSSINFO: 2") != NULL)
    {
      break;
    }

    // Dans certains cas, IGNSSACT devenait faux ?
    // ces lignes forcent IGNSSACT a vrai
    module->sendCommand("AT%IGNSSACT?", "OK\0", reply, 10000);
    if (strstr(reply, "%IGNSSACT: 0") != NULL)
    {
      Serial.print("Activation INGSS ... ");
      module->sendCommand("AT%IGNSSACT=1", "OK\0", 10000);
    }

    delay(500);
    Serial.print(".");
    ++enter;
    if (enter > 100)
    {
      enter = 0;
      Serial.println();
    }

    this->updateLeds();
  } while (1);

  this->led_RX->setEtat(0);
  this->led_RX->setFlashe(0);

  module->sendCommand("AT+CFUN=1", "OK\0", reply, 10000);

  Serial.println(" OK!");
}

bool MGN::lireGNSS(void)
{
  char pos[255] = {0};

  module->sendCommand("AT%IGNSSINFO=\"LASTFIX\"", "OK\0", pos, 1000);
  // Serial.println(pos);

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

    return false;
  }

  /*
  AT%IGNSSINFO="LASTFIX"

  %IGNSSINFO: 2,"20:50:25","29/11/2024","45.404267","-71.892385","196.3",1732913425000,,,"B",6

  OK
  */
  // Reponse si position GPS:
  // %IGNSSINFO: 2,"21:29:14","12/11/2024","45.403975","-71.891657","40.0",1731446954000,,,"B",3
  // OK
  // char pos[100] = "%IGNSSINFO: 2,\"21:29:14\",\"12/11/2024\",\"45.403975\",\"-71.891657\",\"40.0\",1731446954000,,,\"B\",3\x0\xd\x0\xdOK";
  char *ptr = strstr(pos, "%IGNSSINFO:");

  // Position GPS inconnue, ou commande a echoue
  if (ptr == NULL)
  {
    return false;
  }

  char *ptr_latitude = ptr + 39;  // longueur 9
  char *ptr_longitude = ptr + 51; // longueur 10 (a cause du -)

  char char_latitude[11] = "";
  char char_longitude[11] = "";

  uint8_t n = 9;
  bool estNegatif = false;

  if (ptr_latitude[0] == 0x2D)
  {
    estNegatif = true;
    ++ptr_latitude;
  }

  for (uint8_t i = 0; i < n; ++i)
  {
    char_latitude[i] = *(ptr_latitude + i);
    char_longitude[i] = *(ptr_longitude + i);
  }

  this->latitude = atof(char_latitude);
  this->longitude = atof(char_longitude);

  if (estNegatif)
  {
    this->latitude = -this->latitude;
  }

  Serial.print("Nouvelle lat / long : ");
  Serial.print(this->latitude);
  Serial.print(" ");
  Serial.println(this->longitude);

  return true;
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
  uint8_t cmd = this->commande;
  float lat = this->latitude;
  float longitude = this->longitude;
  uint32_t running_minutes = this->running_minutes;
  uint8_t checksum = 0;

  Serial.print("Donnees : ");
  Serial.print(network);
  Serial.print(", ");
  Serial.print(id);
  Serial.print(", ");
  Serial.print(cmd);
  Serial.print(", ");
  Serial.print(lat);
  Serial.print(", ");
  Serial.print(longitude);
  Serial.print(", ");
  Serial.print(running_minutes);
  Serial.println(". ");

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
  payload[0] = id | (network << 5) | (cmd << 6);
  Serial.print("Byte 0 : ");
  Serial.println((uint8_t)this->message[0]);

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
  // Serial.print("\nCalcul checksum : ");
  for (int i = 0; i < 12; ++i)
  {
    checksum += payload[i];
    // Serial.print(checksum);
    // Serial.print(" ");
  }
  // Serial.println();

  payload[12] = checksum;

  // Resultat
  /*Serial.print("\nPayload : ");
  for (int i = 0; i < 12; ++i)
  {
    Serial.print(payload[i]);
    Serial.print(" ");
  }
  Serial.println();*/

  // buffer
  memset(this->message, 0, 63);

  uint8_t index = 0;

  index += sprintf(this->message, "AT%%SOCKETDATA");

  index += sprintf(this->message + index, "=\"SEND\",1,13,\"");

  for (int8_t i = 0; i < 13; ++i)
  {
    index += sprintf(this->message + index, "%02X", payload[i]);
  }

  sprintf(this->message + index, "\"");
}

void MGN::decodeMessage(char *message)
{
  // Format recu :
  // CMD :    01 ou 00
  // set id : 1x
  Serial.print("Message : ");
  Serial.print(message);
  char byte0_str[2] = {0};
  char byte1_str[2] = {0};

  memcpy(byte0_str, &(message[0]), 1);
  memcpy(byte1_str, &(message[1]), 1);

  uint8_t byte0 = atoi(byte0_str);
  uint8_t byte1 = atoi(byte1_str);

  if (byte0 == 0) // CMD
  {
    if (byte1 == 1) // Set
    {
      Serial.println("Allume LED Rx.");
      this->led_RX->setEtat(1);
      this->commande = true;
    }
    else if (byte1 == 0) // Reset
    {
      Serial.println("Eteint LED Rx.");
      this->led_RX->setEtat(0);
      this->commande = false;
    }
  }
  else if (byte0 == 1) // Set ID
  {
    if (byte1 != 0)
    {
      this->setID(byte1);
    }
  }
}

bool MGN::messageRecu(char *reply)
{
  char *ptr = strstr(reply, "%SOCKETDATA:1,");

  if (ptr == NULL)
  {
    // Reply invalide
    Serial.println("Reply invalide.");
    return false;
  }

  // "%SOCKETDATA:1,XX,
  //                ^ pos 14
  ptr = ptr + 14;

  if (ptr[0] == '0')
  {
    // Pas de message recu
    // Serial.println("Aucun caractere recu.");
    return false;
  }

  uint8_t index = 0;
  unsigned char c;
  unsigned char num_char_ascii[2] = {0};
  do
  {
    c = ptr[index];
    if (c == ',')
    {
      break;
    }
    else
    {
      num_char_ascii[index] = ptr[index];
    }
    ++index;
  } while (1);

  if (index == 0)
  {
    Serial.println("Nombre de caracteres invalide.");
    return false;
  }

  // Nombre de caracteres a aller prendre dans la chaine
  uint8_t num_char = atoi(num_char_ascii);

  // Debut de la chaîne est le signe '"'
  if (strstr(reply, "\"") == NULL)
  {
    // Message ne contient pas de '"'
    Serial.println("N'a pas trouve de signe \".");
    return false;
  }

  do
  {
    ++ptr;
  } while (ptr[0] != '\"');

  // prochain caractere est le debut de la chaine
  ptr = ptr + 1;

  char message[255];
  memset(message, 0, 255);

  index = 0;
  do
  {
    c = ptr[index];
    if (c == '"')
    {
      // Fin de la chaîne atteint
      break;
    }
    else
    {
      message[index] = ptr[index];
    }
    ++index;
  } while (index < 255);

  this->decodeMessage(message);

  // Serial.print("Message recu : ");
  // Serial.println(message);
}

void MGN::update(void)
{
  static unsigned long dernier_update = 0;
  updateLeds();
  MGN_RunningHours_update();
  this->running_minutes = MGN_RunningHours_getDigitalInCount();
  delay(10);

  // Delai entre les requetes
  if (dernier_update + TEMPS_UPDATE < millis())
  {

    dernier_update = millis();

    // Boucle de 10 minutes écoulée ?
    if (this->t_debut + this->TEMPS_BOUCLE < millis())
    {
      this->closeSocket();
      Serial.println("Boucle terminee, nouvelle boucle.");
      this->messageEnvoye = false;
      this->messageNTNEnvoye = false;
      this->t_debut = millis();
      this->attendFixGNSS();
      do
      {
      } while (!this->lireGNSS());
      uint8_t i = 0;
      do
      {
        delay(10);
        ++i;
      } while (!this->switchToLTE() && i < 50);
      Serial.print("Attente de connexion ");
    }
    else
    {
      /*Serial.print("Nope : ");
      Serial.print(this->t_debut);
      Serial.print(" + ");
      Serial.print(this->TEMPS_BOUCLE);
      Serial.print(" > ");
      Serial.println(millis());*/
    }

    // Message envoyé ?
    if (!this->messageEnvoye)
    {
      if (this->estConnecte())
      {
        if (this->sendData())
        {
          this->messageEnvoye = 1;
          if(this->reseauActuel == RESEAU_NTN) {
            this->messageNTNEnvoye = true;
          }
        }
        else
        {
          // Echec envoi du message
        }
      }
      Serial.print(".");
    }
    else // Envoi terminé, mode écoute
    {
      if (this->estConnecte())
      {
        if (!this->getData())
        {
          Serial.print(".");
        }
        // Si bouton appuyé, force l'envoi d'un paquet
        if (!digitalRead(BOUTON_PIN))
        {
          Serial.println("Demande envoi paquet.");
          this->closeSocket();
          this->sendData();
        }
      }
      else
      { // Attente connexion (reception)
        Serial.print("-");
      }
    }

    // Moitié du temps écoulé dans la boucle de 10 min
    // Mettre if(1) pour tester la connexion au NTN
    if (this->t_debut + this->TEMPS_BOUCLE / 2 < millis() && this->messageNTNEnvoye == false)
    {
      this->messageEnvoye = false;
      if (this->reseauActuel != RESEAU_NTN)
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
}