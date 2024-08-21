/*
 * File:   CFH.c
 * Author: mfortier
 *
 * Created on May 5, 2023, 4:39 PM
 *     Rev1 : 2024-08-20 - Modifications pour L3-MGN
 */

#include "CFH.h"

#ifdef XC8_TOOLCHAIN
#else // Définitions pour les tests unitaires
uint8_t fakeMem[16];
#endif

// objet temps
struct tm maintenant;
uint32_t maintenant_epoch;
uint32_t prochaineEcriture_epoch;

// flag pour mettre à jour le RTC
bool attenteUpdateRTC = false;

// Pour la routine qui fait le polling du bus 1-wire
bool update1W = false;

/**
 * \brief Flag d'update du bus 1W
 */
void TMR0_2s_ISR() {
    update1W = true;
}

/**
 * \brief Vérifie si une nouvelle cartouche est présente et choisit l'action à faire
 * selon son type
 */
void pollingCartouche(void) {

    // contenant pour l'adresse de la cartouche
    uint8_t cartouche[8] = {0};

    // Flag du premier cycle
    static bool premierCycle = true;

    // Au démarrage, lit l'adresse unique du RTC et l'inscrit dans la mémoire locale
    if (premierCycle) {
        setIdBase();
        premierCycle = false;
    }

    // Effectue la mise à jour demandée au passage précédent
    if (attenteUpdateRTC) {
        MCP7941X_setTime(&maintenant);
        attenteUpdateRTC = false;
        setEtatLED(etat_configDone);
    }

    // Mémorisation qu'une cartouche RTC a déjà été utilisée
    static bool dejaFait = false;

    // Lecture du temps en prévision de l'écriture dans le iButton
    MCP7941X_getTime(&maintenant);
    maintenant_epoch = (uint32_t) mktime(&maintenant);

    // Détection de la cartouche et lecture du ROM
    uint8_t test = ow_reset();
    if (test) {
        // Lire l'information sur le périphérique
        if (!ow_readROM_safe(cartouche)) {
            // CRC invalide (cartouche corrompue, problème de comm?)
            setEtatLED(etat_configuration);
            dejaFait = false;
            return;
        }
    } else {
        setEtatLED(etat_standby);
        dejaFait = false; // Mémorisation pour config RTC
        return;
    }
    
    test = do_crc(cartouche, 8*sizeof(uint8_t));
    
    if(test != 0) {
        return;
    }

    switch (cartouche[0]) {
        case OW_FAMILY_DS1904:
            // Est-ce que le temps a déjà été mis à jour?
            if (!dejaFait) {
                setEtatLED(etat_configuration);
                if (executerCartoucheRTC()) {
                    dejaFait = true;
                } else {
                    //setEtatLED(etat_erreur);
                    dejaFait = false;
                }
            }
            break;
        case OW_FAMILY_DS1972:
            // EEPROM 128B
        case OW_FAMILY_DS1973:
            // EEPROM 512B
            setEtatLED(etat_operation);
            executerCartoucheROM(cartouche);
            dejaFait = false;
            break;
        case OW_FAMILY_DS1977:
            // EEPROM 32KB
        case OW_FAMILY_INVALID:
        default:
            //setEtatLED(etat_erreur);
            dejaFait = false;
            break;
    }
}

/**
 * \brief Lecture du serial sauvegardé dans le EEPROM local
 * @param cartouche variable qui contiendra le ROM de la cartouche en cours
 */
void getCartoucheInstallee(uint8_t * cartouche) {
    for (uint8_t i = 0; i < 8; ++i) {
        cartouche[i] = EEPROM_Read(MEMORY_ADDRESS_LAST_CARTRIDGE_SERIAL + i);
    }
}

/**
 * \brief Écriture du ROM de la cartouche dans la base
 * @param pCartouche Pointeur vers le array contenant le rom de la cartouche
 */
void setCartoucheInstallee(uint8_t * pCartouche) {
    uint8_t byte;
    // Écriture des 8 bytes de l'adresse
    for (uint8_t i = 0; i < 8; ++i) {
        byte = pCartouche[i];
        EEPROM_Write(MEMORY_ADDRESS_LAST_CARTRIDGE_SERIAL + i, byte);
    }
}

/**
 * \brief Teste si la cartouche correspond à la dernière cartouche lue
 * @param nouvelleCartouche
 * @return True si la cartouche correspond
 */
bool cartoucheDejaInstallee(uint8_t * nouvelleCartouche) {
    // ID de la cartouche déjà sauvegardée
    uint8_t cartoucheInstallee[8];

    getCartoucheInstallee(cartoucheInstallee);

    // Est-ce que les deux cartouches sont identiques?
    // memcmp retourne zero si ==
    if (!memcmp(nouvelleCartouche, &cartoucheInstallee, 8 * sizeof (uint8_t))) {

        /*uint8_t IUBCartouche[8] = {0};
        uint8_t iub[8] = {0};

        lireIUBcartouche(nouvelleCartouche, IUBCartouche);
        getIdBase(iub);

        // IUB dans la cartouche = iub
        if (!memcmp(&IUBCartouche, &iub, 8 * sizeof (uint8_t))) {
            // ... et date installation valide
            if (lireDateInstallee(nouvelleCartouche) != 0) {

                return true;
            }
        }*/
        return true;
    }
    // Nouvelle cartouche ou infos invalides
    return false;
}

/**
 * \brief Gestion à la détection d'une cartouche de type ROM
 * @param nouvelleCartouche
 */
void executerCartoucheROM(uint8_t * nouvelleCartouche) {
    // Vérifie s'il s'agit d'une nouvelle cartouche
    if (!cartoucheDejaInstallee(nouvelleCartouche)) {
        // Sauvegarde locale du serial de la nouvelle cartouche
        setCartoucheInstallee(nouvelleCartouche);
        setEtatLED(etat_erreur);

        // Écriture initiale des paramètres dans la nouvelle cartouche
        ecrireNouvelleCartouche();
    } else if (maintenant_epoch > prochaineEcriture_epoch) {
        // Mise à jour 'date removed' dans la cartouche

        ecrireDateRetrait(nouvelleCartouche);

        // prochain traitement après l'attente définie
        prochaineEcriture_epoch = maintenant_epoch + NB_MINUTES_ATTENTE * 60;
    }
}

/**
 * \brief Gestion cartouche de type RTC pour configurer date/heure de la base
 */
bool executerCartoucheRTC() {
    // Si un temps valide est lu, mettre à jour le RTC de la base
    if (ow_readClock(&maintenant)) {
        attenteUpdateRTC = true;
        return true;
    } else {

        return false;
    }
}

/**
 * \brief Mise à jour de la date de retrait sur la cartouche
 * @param p_pCartridge
 * @return True si l'écriture a fonctionné
 */
bool ecrireDateRetrait(uint8_t *p_pCartridge) {
    uint32_t uValue = 0;
    uint8_t buffer[8] = {0};
    uint8_t dateRemoved[4] = {0};
    uint8_t writesCount[4] = {0};
    bool success = true;

    // Date removed est écrit a deux emplacements mémoire

    //------------------------------------
    //------------------------------------
    // Premier bloc :
    // - Writes count (4 bytes)
    // - Date removes (4 bytes)

    // Lecture et incrémentation du nombre actuel d'écritures, 
    // à placer dans le buffer [0:3]
    ow_readMemory_Safe(p_pCartridge, CARTRIDGE_WRITES_COUNT2, &uValue, sizeof (uint32_t));
    uValue = flip(uValue) + 1;
    uValue = flip(uValue);
    for (uint8_t i = 0; i < 4; ++i) {
        buffer[i] = ((uint8_t *) & uValue)[i];
        writesCount[i] = buffer[i];
    }

    // Lecture du temps actuel, à placer dans le buffer [4:7]
    uint32_t uValue2 = 0;
    uValue2 = maintenant_epoch;
    for (uint8_t i = 0; i < 4; ++i) {
        buffer[i + 4] = ((uint8_t *) & uValue2)[i];
        dateRemoved[i] = buffer[i + 4]; // En prévision du calcul du crc
    }

    // Premiere écriture
    success &= ow_write8bytes(CARTRIDGE_WRITES_COUNT2, buffer);

    //------------------------------------
    //------------------------------------
    // Deuxième bloc :
    // - Date installée (4 bytes)
    // - Date retrait (4 bytes) - deja dans le buffer

    // Lecture de la date installée, à placer dans le buffer [0:3]
    uValue = lireDateInstallee(p_pCartridge);
    for (uint8_t i = 0; i < 4; ++i) {
        buffer[i] = ((uint8_t *) & uValue)[i];
    }

    // buffer[4:7] contient deja date removed

    // Écriture à l'adresse principale
    success &= ow_write8bytes(CARTRIDGE_ADDRESS_DATE_INSTALLED, buffer);

    //------------------------------------
    //------------------------------------
    // Troisième bloc :
    uint8_t crc_dateRemoved = do_crc(dateRemoved, sizeof (uint32_t));
    uint8_t crc_writesCount = do_crc(writesCount, sizeof (uint32_t));

    for (uint8_t i = 0; i < 8; ++i) {
        buffer[i] = 0;
    }

    buffer[0] = crc_writesCount;
    buffer[1] = crc_dateRemoved;

    // Écriture à l'adresse principale
    success &= ow_write8bytes(CARTRIDGE_WRITES_COUNT_CRC, buffer);

    return success;
}

/**
 * \brief Écriture de toutes les données (seulement pour une nouvelle cartouche)
 * @param pCartouche ROM de la cartouche à écrire
 * @param pBase UID de la base
 */
void ecrireNouvelleCartouche() {
    uint8_t success;
    uint32_t uValue;

    // Buffer de 8 bytes qui contiendra les données
    uint8_t buffer[8] = {0};

    // Date d'installation
    uint8_t dateInstallation[4] = {0};
    uValue = maintenant_epoch;

    for (uint8_t i = 0; i < 4; ++i) {
        dateInstallation[i] = ((uint8_t *) & uValue)[i];
    }

    // Identifiant unique de la carte
    uint8_t iub[8] = {0};
    getIdBase(iub);

    // À écrire:
    // - host model
    // - hardware rev
    // - software rev
    // - date installed
    // - iub
    // - iub 2 (copie qui n'est pas segmentée)
    // - crc pour date installed
    // - crc pour iub
    // Segmenter en écritures de 8 bytes
    // Contrainte : respecter les adresses originales de la révision initiale

    //------------------------------------
    //------------------------------------
    // Premier bloc de 8 bytes : 
    // - host model (1 byte)
    // - hardware rev (1 byte)
    // - software rev (1 byte)
    // - iub[0:4] (5 premiers bytes de iub)
    // Écriture à partir de la première adresse, qui est celle du modèle
    buffer[0] = MODEL;
    buffer[1] = HARDWARE_REVISION;
    buffer[2] = SOFTWARE_REVISION;
    for (uint8_t i = 0; i < 5; ++i) {
        buffer[i + 3] = iub[i]; // bytes 3 à 7 du buffer, 0 à 4 du iub
    }

    // Première écriture
    success = 0;
    //do {
    success = ow_write8bytes(CARTRIDGE_ADDRESS_HOST_MODEL, buffer);
    //} while (!success);

    //------------------------------------
    //------------------------------------
    // Deuxième bloc de 8 bytes:
    // - iub[4:7] (bytes 5 à 8 de iub)
    // - le reste = 0
    for (uint8_t i = 0; i < 8; ++i) {
        buffer[i] = 0; // le reste = 0
    }

    for (uint8_t i = 0; i < 3; ++i) {
        buffer[i] = iub[i + 5]; // bytes 0 à 2 du buffer, 5 à 7 du iub
    }

    // Deuxieme ecriture
    success = 0;
    //do {
    success = ow_write8bytes(CARTRIDGE_ADDRESS_HOST_MODEL + 8, buffer);
    //} while (!success);


    //------------------------------------
    //------------------------------------
    // Troisième bloc de 8 bytes:
    // - Date installed (4 bytes)
    // - le reste = 0
    for (uint8_t i = 0; i < 8; ++i) {
        buffer[i] = 0; // le reste = 0
    }

    for (uint8_t i = 0; i < 4; ++i) {
        buffer[i] = dateInstallation[i];
    }

    // Troisieme écriture
    success = 0;
    //do {
    success = ow_write8bytes(CARTRIDGE_ADDRESS_DATE_INSTALLED, buffer);
    //} while (!success);
    // Écriture 2e emplacement mémoire pour redondance. Les lectures suivantes
    // visent cet emplacement, et pas CARTRIDGE_ADDRESS_DATE_INSTALLED.
    success = 0;
    //do {
    success = ow_write8bytes(CARTRIDGE_ADDRESS_DATE_INSTALLED2, buffer);
    //} while (!success);

    //------------------------------------
    //------------------------------------
    // Quatrième bloc de 8 bytes : 
    // iub_2, le numéro de série unique de la base, 2e emplacement mémoire
    success = 0;
    //do {
    success = ow_write8bytes(CARTRIDGE_ADDRESS_HOST_SERIAL2, iub); // redondance
    //} while (!success);
    //------------------------------------
    //------------------------------------
    // Cinquieme bloc de 8 bytes :
    // crc du iub (1 byte)
    // crc de Data Installed (1 byte)

    // Calcul des CRC pour iub et date installed
    uint8_t crc_iub = do_crc(iub, 8 * sizeof (uint8_t));
    uint8_t crc_dateInstalled = do_crc(dateInstallation, sizeof (uint32_t));

    for (uint8_t i = 0; i < 8; ++i) {

        buffer[i] = 0; // le reste = 0
    }

    buffer[0] = crc_iub;
    buffer[1] = crc_dateInstalled; // Adresse suivante de crc iub

    // Écriture des crc
    success = 0;
    //do {
    success = ow_write8bytes(CARTRIDGE_ADDRESS_HOST_SERIAL_CRC, buffer);
    //} while (!success);
    // Commande d'écriture de la date de retrait au prochain cycle
    prochaineEcriture_epoch = maintenant_epoch;
    
    if(success){
    	printf("Success.\n");
    }
}

/**
 * \brief Retourne la date d'installation écrite dans la cartouche
 * @param p_pCartridge
 * @return 
 */
uint32_t lireDateInstallee(uint8_t * p_pCartridge) {
    uint32_t uValue;

    ow_readMemory_Safe(p_pCartridge, CARTRIDGE_ADDRESS_DATE_INSTALLED2, &uValue, sizeof (uint32_t));

    //uValue = flip(uValue);

    return uValue;
}

/**
 * \brief Retourne le IUB inscrit dans la cartouche
 * @param p_pCartridge
 * @param array -> le contenant pour le IUB
 * @return 
 */
void lireIUBcartouche(uint8_t * p_pCartridge, uint8_t * array) {

    ow_readMemory_Safe(p_pCartridge, CARTRIDGE_ADDRESS_HOST_SERIAL2, array, 8 * sizeof (uint8_t));
}

/**
 * \brief Inverse l'ordre des bits pour l'écriture dans le iButton
 * @param p_uValue
 * @return 
 */
uint32_t flip(uint32_t p_uValue) {
    uint32_t uValue = 0;

    ((uint8_t *) & uValue)[0] = ((uint8_t *) & p_uValue)[3];
    ((uint8_t *) & uValue)[1] = ((uint8_t *) & p_uValue)[2];
    ((uint8_t *) & uValue)[2] = ((uint8_t *) & p_uValue)[1];
    ((uint8_t *) & uValue)[3] = ((uint8_t *) & p_uValue)[0];

    return uValue;
}
/*
void CFH_eraseROM() {
    uint8_t dataWrite[8] = {0};

    if (ow_reset()) {
        for (uint8_t i = 0; i < 16; ++i) {
            ow_write8bytes(i * 8, &dataWrite);
        }
    }
}*/

/*
void CFH_runTests() {
    uint8_t testRom[8] = {0};

    uint8_t dataWrite[8] = {0}; // = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t dataRead[8] = {0};

    uint32_t err = 0;
    uint32_t invalid = 0;

    if (ow_reset()) {

        ow_readROM(testRom);

        for (uint8_t i = 0; i < 16; ++i) {
            for (uint8_t j = 0; j < 8; ++j) {
                dataWrite[j] = 0;
            }

            ow_write8bytes(i * 8, &dataWrite);
            ow_readMemory(testRom, i * 8, dataRead, 8);
        }

        for (uint16_t i = 0; i < 16; ++i) {
            for (uint16_t j = 0; j < 8; ++j) {
                dataWrite[j] = j + i * 8;
            }

            while (1) {
                if (!ow_write8bytes(i * 8, &dataWrite)) {
                    invalid = invalid + 1;
                }

                ow_readMemory(testRom, i * 8, dataRead, 8);

                if (memcmp(&dataWrite, &dataRead, 8 * sizeof (uint8_t))) {
                    err = err + 1;
                } else {
                    break;
                }
            }
        }

        if (err != 0)
            err = err + 1;
    }
}*/
