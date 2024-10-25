/* 
 * File: 1-wire.c
 * Author: MFortier
 * Comments:
 * Revision history:
 *      Initial : 2023-04-03
 *      Rev1 : 2024-08-20 - Modifications pour L3-MGN
 */

#include "1-wire.h"

#ifdef TDD_SOFTWARE
uint8_t fake1wPinValue = 0;
uint8_t fake1wPinDirection = 0;
#endif

const uint8_t OW_CRC_TABLE[] = {
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

/**
 *  \brief Relâche le bus 1-wire (à Vdd)
 */
void ow_release() {
    IO_1W_SetDigitalInput(); // laisser la sortie en haute impédance
    IO_1W_SetHigh();
}

/**
 *  \brief Maintient le bus 1-wire (à 0V)
 */
void ow_drive() {
    IO_1W_SetDigitalOutput();
    IO_1W_SetLow();
}

/**
 *  \brief Commande de reset 1-wire, retourne True si un périphérique est présent
 * @return True si un périphérique est présent
 */
bool ow_reset() {
    unsigned char presence_detect;

    ow_drive();
    __delay_us(480);
    ow_release();
    __delay_us(70);
    presence_detect = read_OW();
    __delay_us(410);
    ow_release();

    return !presence_detect;
}

/**
 *  \brief Lecture du bus 1-wire
 *  @return La valeur lue sur le bus
 */
bool read_OW() {
    IO_1W_SetDigitalInput();
    return IO_1W_GetValue();
}

/**
 *  \brief Écriture d'un bit sur le bus 1-wire
 *  @param p_bValue le bit à écrire
 */
void ow_writeBit(bool p_bValue) {
    if (p_bValue) {
        ow_drive();
        __delay_us(6);
        ow_release();
        __delay_us(64);
    } else {
        ow_drive();
        __delay_us(60);
        ow_release();
        __delay_us(10);
    }
}

/**
 *  \brief Lecture d'un bit sur le bus 1-wire
 *  @return Le bit lu
 */
bool ow_readBit() {
    bool bValue;

    ow_drive();
    __delay_us(6);
    ow_release();
    __delay_us(9);

    bValue = IO_1W_GetValue();

    __delay_us(55);

    return bValue;
}

/**
 *  \brief Écriture d'un octet sur le bus 1-wire
 *  @param p_byte l'octet à écrire
 */
void ow_writeByte(uint8_t p_byte) {
    for (uint8_t i = 0; i < 8; ++i) {
        ow_writeBit(p_byte & 0x01);
        p_byte >>= 1;
    }
}

/**
 *  \brief Lecture d'un octet sur le bus 1-wire
 *  @param  p_byte l'octet à lire
 *  @return L'octet lu
 */
uint8_t ow_readByte() {
    uint8_t ucValue = 0;

    for (uint8_t i = 0; i < 8; ++i) {
        ucValue >>= 1;

        if (ow_readBit()) {
            ucValue |= 0x80;
        }
    }

    return ucValue;
}

/**
 * \brief Vérification de la validité de l'adresse par CRC8 Maxim
 * @param address Adresse du iButton à vérifier
 * @return true si le crc est valide, false sinon
 */
uint8_t do_crc(uint8_t * address, uint8_t p_uLength) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < p_uLength; ++i) {
        uint8_t val = address[i];
        crc = OW_CRC_TABLE[crc ^ val];
        //printf("CRC : apres %i = %i\n", address[7-i], crc);
    }
    return crc;
}

/**
 * \brief lecture avec vérification du CRC
 * @param array Adresse du iButton
 */
bool ow_readROM_safe(uint8_t * array) {
    bool crc_ok = false;
    uint8_t essais = 0;

    // On s'assure d'avoir un CRC ok avant de retourner l'adresse
    do {
        ++essais;
        ow_readROM(array);
        crc_ok = do_crc(array, 8 * sizeof (uint8_t)) == 0;
    } while (!crc_ok || essais > NB_ESSAIS_LECTURE_MAX);

    return crc_ok;
}

/**
 *  \brief Lecture de l'adresse du périphérique 1-wire
 *  @param array Tableau de 8 octets
 */
void ow_readROM(uint8_t * array) {

    ow_reset();

    // Demande de lecture du ROM au périphérique
    ow_writeByte(OW_COMMAND_READ_ROM);

    // Lecture des octets
    for (uint8_t nByte = 0; nByte < 8; nByte++) {
        array[nByte] = ow_readByte();
    }
}

/**
 * \brief Commande de skip (pour éviter le match ROM)
 */
void ow_skipROM() {
    ow_writeByte(OW_COMMAND_SKIP_ROM);
}

/**
 * \brief Calcul du CRC attendu pour une adresse donnée
 * @param addr  Adresse à calculer
 * @param len   Nombre de byte
 * @return  La valeur calculée du CRC
 *//*
uint8_t crc8(uint8_t *addr, uint8_t len) {
    uint8_t crc = 0;

    for (uint8_t i = 0; i < len; i++) {
        uint8_t inbyte = addr[i];
        for (uint8_t j = 0; j < 8; j++) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix)
                crc ^= 0x8C;

            inbyte >>= 1;
        }
    }
    return crc;
}*/

/**
 * \brief Écriture de 8 bytes en mémoire du périphérique 1-wire
 * @param p_uAddress
 * @param p_pData
 * @return 
 */
bool ow_write8bytes(uint16_t p_uAddress, uint8_t * p_pData) {
    uint8_t uByte;
    uint8_t nbBytes = 8;
    unsigned char ucResponse;
    uint8_t uValue = 0;
    bool bValid = true;
    uint8_t numEssai = 0;

    do {
        uValue = 0;
        bValid = true;

        ++numEssai;
        if (numEssai > NB_ESSAIS_ECRITURE_MAX) {
            printf("\tEcriture a echoue : trop d'essais.\n");
            return false;
        }

        ow_reset();
        ow_skipROM();

        // Écriture dans le scratchpad
        ow_writeByte(OW_COMMAND_WRITE_SCRATCHPAD);
        ow_writeByte((uint8_t)p_uAddress & 0xFF);
        ow_writeByte((uint8_t)(p_uAddress >> 8) & 0xFF);

        for (uByte = 0; uByte < nbBytes; uByte++) {
            ow_writeByte(p_pData[uByte]);
        }

        // Vérification des données écrites
        ow_reset();
        ow_skipROM();
        ow_writeByte(OW_COMMAND_READ_SCRATCHPAD);

        // Le périphérique envoie d'abord l'adresse
        uValue = ow_readByte();
        uValue |= ow_readByte() << 8;

        // Vérification que l'adresse correspond
        if (uValue != p_uAddress) {
            printf("\tEcriture a echoue : adresse ne correspond pas.\n");
            bValid = false;
        }

        // Ensuite le périphérique envoie le "ending offset/data status byte (E/S)"
        ucResponse = ow_readByte();

        // Ensuite ce sont les données
        for (uByte = 0; uByte < nbBytes; uByte++) {
            uValue = ow_readByte();
            // Si le byte dans le scratchpad ne correspond pas, erreur
            // Ceci peut être dû à une adresse invalide ou protégée
            if (uValue != p_pData[uByte]) {
                bValid = false;
                printf("\tErreur : les donnees ne correspondent pas.\n");
            }
        }

        // Si l'adresse et les données étaient bonnes, envoie de la commande
        // de copie du scratchpad en mémoire à l'adresse voulue
        if (bValid) {
            ow_reset();
            ow_skipROM();

            // Envoie de la commande ... 
            ow_writeByte(OW_DS1972_COMMAND_COPY_SCRATCHPAD);

            // ... de l'adresse ... 
            ow_writeByte((uint8_t)p_uAddress & 0xFF);
            ow_writeByte((uint8_t)(p_uAddress >> 8) & 0xFF);

            // ... et du "ending offset/data status byte (E/S)"
            ow_writeByte(ucResponse);

            // Le bus doit être en haut impédance pendant tPROG, qui est 10ms
            // On se donne un peu de buffer
            ow_release();
            __delay_ms(10);

            ucResponse = ow_readByte();

            // Si on commence à lire trop tard, pourrait être 0x55 ?
            // (0101 0101 au lieu de 1010 1010 ?)
            if (ucResponse != 0xAA) {// || ucResponse != 0x55) {
                bValid = false;
                printf("\tEcriture a echoue : reponse invalide.\n");
            }
        }
    } while (!bValid);
    
    printf(".");

    return bValid;
}

/**
 * \brief Fonction qui écrit dans la mémoire du périphérique. Importée du code original et adaptée
 * pour permettre une écriture par ligne.
 * @param p_pROM        L'adresse du périphérique
 * @param p_uAddress    L'adresse du registre
 * @param p_pData       Un pointeur vers les données à écrire
 * @param p_uLength     Le nombre d'octets à écrire
 * @return 
 *//*
bool ow_writeMemory(uint8_t * p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength) {
    uint8_t uByte;
    unsigned char ucResponse;
    uint16_t uValue;
    bool bValid = true;
    uint8_t uPageSize;
    uint8_t uScratchPadCommand;
    uint8_t uLength;
    uint8_t uPage = 0;
    bool bPassword = false;
    uint16_t uAddress;
    uint8_t uOffset;
    uint8_t expandL = 0;
    uint8_t expandR = 0;
    uint16_t startAddress = 0;
    uint8_t aData[16] = {0};
    uint8_t newSize = 0;


    // Doit corriger pour lire seulement des blocs de 16 bits
    expandL = 16 - (p_uAddress % 16);
    expandR = 16 - (p_uLength + expandL) % 16;

    if (expandR != 0 || expandL != 0) {
        ow_readMemory(p_pROM, startAddress, &aData, 16 * sizeof (uint8_t));
    }

    // On copie les données par dessus la lecture des 16 bits si applicable
    for (uint8_t i = 0; i < p_uLength; ++i) {
        aData[i + expandL] = ((uint8_t *) p_pData)[i];
    }

    startAddress = p_uAddress - expandL;
    newSize = p_uLength + expandL + expandR;

    switch (p_pROM[0]) {
        case OW_FAMILY_DS1972:
        case OW_FAMILY_DS1973:
            uScratchPadCommand = OW_DS1972_COMMAND_COPY_SCRATCHPAD;
            uPageSize = 8;


            break;
        case OW_FAMILY_DS1977:
            uScratchPadCommand = OW_DS1977_COMMAND_COPY_SCRATCHPAD;
            uPageSize = 64;
            bPassword = true;
            break;
    }

    ow_reset();
    ow_skipROM();

    // Send bytes to the scratchpad
    ow_writeByte(OW_COMMAND_WRITE_SCRATCHPAD);
    ow_writeByte(startAddress & 0xFF);
    ow_writeByte((startAddress >> 8) & 0xFF);

    for (uByte = 0; uByte < newSize; uByte++) {
        ow_writeByte(aData[uByte]);
    }

    // Verify the written bytes
    ow_reset();
    ow_skipROM();
    ow_writeByte(OW_COMMAND_READ_SCRATCHPAD);

    uValue = ow_readByte();
    uValue |= ow_readByte() << 8;
    if (uValue != startAddress) {
        bValid = false;
        // printf("ERROR 1\r\n");
    }
    ucResponse = ow_readByte();

    for (uByte = 0; uByte < newSize; uByte++) {
        uValue = ow_readByte();
    }

    // If validated, copy to the memory
    if (bValid) {
        ow_reset();
        ow_skipROM();
        ow_writeByte(uScratchPadCommand);
        ow_writeByte(startAddress & 0xFF);
        ow_writeByte((startAddress >> 8) & 0xFF);
        ow_writeByte(ucResponse);

        if (bPassword) {
            // Send dummy password
            for (uByte = 0; uByte < 8; uByte++) {
                ow_writeByte(0x00);
            }
        }

        // Keep the bus powered for the programming time
        ow_release();
        __delay_ms(10);

        ucResponse = ow_readByte();

        if (ucResponse != 0xAA) {
            bValid = false;
        }
    }

#ifdef DEBUG
    if (!bValid) {
        printf("ow_writeMemory() error\r\n");
    }
#endif

    return bValid;
}*/

void ow_readMemory_Safe(uint8_t * p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength) {
    uint16_t addresse_crc = 0;
    uint8_t crc = 0;
    uint8_t flag_crc = 1;
    uint8_t nbEssais = 0;

    if (p_uAddress == CARTRIDGE_ADDRESS_HOST_SERIAL2) {
        addresse_crc = CARTRIDGE_ADDRESS_HOST_SERIAL_CRC;
    } else if (p_uAddress == CARTRIDGE_ADDRESS_DATE_INSTALLED2) {
        addresse_crc = CARTRIDGE_ADDRESS_DATE_INSTALLED_CRC;
    } else if (p_uAddress == CARTRIDGE_WRITES_COUNT2) {
        addresse_crc = CARTRIDGE_WRITES_COUNT_CRC;
    } else if (p_uAddress == CARTRIDGE_ADDRESS_DATE_REMOVED) {
        addresse_crc = CARTRIDGE_ADDRESS_DATE_REMOVED_CRC;
    } else {
        flag_crc = 0;
    }

    do {
        ++nbEssais;
        // Lecture de l'emplacement mémoire
        ow_readMemory(p_pROM, p_uAddress, p_pData, p_uLength);

        if (flag_crc) {
            ow_readMemory(p_pROM, addresse_crc, &crc, sizeof (uint8_t));
        }
    } while (flag_crc && (crc != do_crc(p_pData, p_uLength)) && (nbEssais < NB_ESSAIS_LECTURE_MAX));
}

/**
 * \brief Fonction pour lire un bloc mémoire d'un périphérique, adaptée du code original
 * pour permettre un accès avec un array.
 * @param p_pROM        L'adresse du périphérique
 * @param p_uAddress    L'adresse du registre
 * @param p_pData       Un pointeur vers les données à écrire
 * @param p_uLength     Le nombre d'octets à écrire
 */
void ow_readMemory(uint8_t * p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength) {
    uint8_t aData[16] = {0};
    uint8_t bufData[16] = {0};

    uint8_t uByte;
    uint8_t uReadCommand = 0x00;
    bool bPassword = false;
    bool bChecksum = false;
    bool bPower = false;
    uint8_t uPageSize;
    uint8_t expandL = 0;
    uint8_t expandR = 0;
    uint16_t startAddress = 0;

    // Doit corriger pour lire seulement des blocs de 8 byte
    if (p_uAddress % 16 != 0) {
        expandL = p_uAddress % 16;
    }
    if ((p_uLength + expandL) % 16 != 0) {
        expandR = 16 - (p_uLength + expandL) % 16;
    }

    startAddress = p_uAddress - expandL;

    uint8_t famille = p_pROM[0];

    switch (famille) {
        case OW_FAMILY_DS1972:
        case OW_FAMILY_DS1973:
            uReadCommand = OW_COMMAND_READ_MEMORY;
            uPageSize = 144;

            // There is no checksum so we can break the read before the end
            if (p_uLength < uPageSize) {
                uPageSize = (uint8_t)(p_uLength + expandL + expandR);
            }
            break;
        case OW_FAMILY_DS1977:
            uReadCommand = OW_COMMAND_READ_MEMORY_WITH_PASSWORD;
            bPassword = true;
            bPower = true;
            bChecksum = true;
            uPageSize = 66;
            break;
        default:
            break;
    }

    ow_reset();
    ow_skipROM();
    ow_writeByte(uReadCommand);
    ow_writeByte((uint8_t)startAddress & 0xFF);
    ow_writeByte((uint8_t)(startAddress >> 8) & 0xFF);

    if (bPassword) {
        // Supply the password
        for (uByte = 0; uByte < 8; uByte++) {
            ow_writeByte(0x00);
        }
    }

    if (bPower) {
        // Keep the bus powered for the reading time
        ow_release();
        __delay_ms(10);
    }

    // Read page including CRC
    for (uByte = 0; uByte < uPageSize; uByte++) {
        aData[uByte] = ow_readByte();
    }

    if (bChecksum) {
        // \todo Verify checksum
    }

    for (uint8_t i = 0; i < p_uLength; ++i) {
        bufData[i] = aData[i + expandL];
    }

    // Copy required data
    memcpy(p_pData, bufData, p_uLength);
}

/**
 * \brief Fonction pour lire le temps actuel dans un périphérique RTC
 * @param p_pDateTime   L'objet dateTime qui sera rempli
 * @return  True si le nombre de secondes est > à 1000
 */
bool ow_readClock(struct tm * p_pDateTime) {

    time_t epoch = 0;
    uint8_t epoch_array[4] = {0};

    // Début de la transaction
    ow_reset();
    ow_skipROM();

    // Demande de lecture du temps RTC
    ow_writeByte(OW_COMMAND_READ_CLOCK);

    // Premier byte sera écrasé
    epoch_array[0] = ow_readByte();

    // Les 4 bytes suivants sont le temps epoch
    for (uint8_t i = 0; i < 4; ++i) {
        epoch_array[i] = ow_readByte();
    }

    // Copie des bytes qui forment un int 32bits
    // représentant le epoch time UNIX
    memcpy(&epoch, &epoch_array, 4 * sizeof (uint8_t));

    // Fin de la transaction
    ow_reset();

    struct tm * test = gmtime(&epoch);

    memcpy(p_pDateTime, test, sizeof (struct tm));

    // Vérifie que le temps est valide
    return epoch > 1000;
}
/**
 * \brief Méthode pour parler avec un périphérique qui n'est pas utilisée
 * @param p_pROM
 *//*
void ow_matchROM(ow_rom *p_pROM) {
    int nByte;

    ow_writeByte(OW_COMMAND_MATCH_ROM);

    ow_writeByte(p_pROM->familyCode);
    p_pROM->serialNumber = 0;

    for (nByte = 5; nByte >= 0; nByte--) {
        //ulByte = ow_readByte(p_uPin);

        //p_pROM->serialNumber |= ulByte << nByte * 8;
    }

    p_pROM->crc = ow_readByte();
}*/
/**
 * \brief Function importée du code original, non utilisée
 * @param p_pROM
 * @return 
 *//*
bool ow_testCRC(ow_rom *p_pROM) {
    uint8_t aROM[8];
    uint8_t uCRC = 0, uTemp, uMix, uByte, uBit;
#define POLYVAL 0x8C

    ow_romToArray(p_pROM, aROM, 0);

    for (uByte = 0; uByte < 7; uByte++) {
        uTemp = aROM[uByte];

        for (uBit = 0; uBit < 8; uBit++) {
            uMix = (uCRC ^ uTemp) & 0x01;

            uCRC >>= 1;

            if (uMix) {
                uCRC ^= POLYVAL;
            }

            uTemp >>= 1;
        }
    }

    uCRC = uCRC;

    return uCRC == aROM[7];
}*/

/**
 * \brief Autre fonction importée du code original, non utilisée
 * @param new
 * @param crc8
 *//*
void docrc8(unsigned char new, unsigned char *crc8) {
#define POLYVAL 0x8C // Polynome qui correspond au standard 1-Wire.
    unsigned char c, i;
    c = *crc8;
    for (i = 0; i < 8; i++) {
        if ((c ^ new) & 1)
            c = (c >> 1) ^ POLYVAL;
        else
            c >>= 1;
        new >>= 1;
    }
    *crc8 = c;
    // Testée et fonctionnelle le 10 mars 2010 AE.
}*/


// p_uAddress Must be multiple of 8

/**
 * \brief Façon d'écrire originale, qui écrit l'ensemble des données chaque fois. non utilisée.
 * @param p_pROM
 * @param p_uAddress
 * @param p_pData
 * @param p_uLength
 * @return 
 *//*
bool ow_writeMemory_old(ow_rom *p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength) {
    uint8_t uByte;
    unsigned char ucResponse;
    uint16_t uValue;
    bool bValid = true;
    uint8_t uPageSize;
    uint8_t uScratchPadCommand;
    uint8_t uLength;
    uint8_t uPage = 0;
    bool bPassword = false;
    uint16_t uAddress;
    uint8_t uOffset;

    switch (p_pROM->familyCode) {
        case OW_FAMILY_DS1972:
        case OW_FAMILY_DS1973:
            uScratchPadCommand = OW_DS1972_COMMAND_COPY_SCRATCHPAD;
            uPageSize = 8;

            // Pad length to 8 bytes multiple
            if (p_uLength % 8 > 0) {
                p_uLength = (p_uLength / 8) * 8 + 8;
            }
            break;
        case OW_FAMILY_DS1977:
            uScratchPadCommand = OW_DS1977_COMMAND_COPY_SCRATCHPAD;
            uPageSize = 64;
            bPassword = true;
            break;
    }

    do {
        uAddress = p_uAddress + uPage * uPageSize;
        uOffset = uPage * uPageSize;

        if (uAddress + uPageSize <= p_uAddress + p_uLength) {
            uLength = uPageSize;
        } else {
            uLength = p_uAddress + p_uLength - uAddress;
        }

        ow_reset();
        ow_skipROM();

        // Send bytes to the scratchpad
        ow_writeByte(OW_COMMAND_WRITE_SCRATCHPAD);
        ow_writeByte(uAddress & 0xFF);
        ow_writeByte((uAddress >> 8) & 0xFF);

        for (uByte = 0; uByte < uLength; uByte++) {
            ow_writeByte(((uint8_t *) p_pData)[uOffset + uByte]);
        }

        // Verify the written bytes
        ow_reset();
        ow_skipROM();
        ow_writeByte(OW_COMMAND_READ_SCRATCHPAD);

        uValue = ow_readByte();
        uValue |= ow_readByte() << 8;
        if (uValue != uAddress) {
            bValid = false;
           // printf("ERROR 1\r\n");
        }
        ucResponse = ow_readByte();

        for (uByte = 0; uByte < uLength; uByte++) {
            uValue = ow_readByte();
        }

        // If validated, copy to the memory
        if (bValid) {
            ow_reset();
            ow_skipROM();
            ow_writeByte(uScratchPadCommand);
            ow_writeByte(uAddress & 0xFF);
            ow_writeByte((uAddress >> 8) & 0xFF);
            ow_writeByte(ucResponse);

            if (bPassword) {
                // Send dummy password
                for (uByte = 0; uByte < 8; uByte++) {
                    ow_writeByte(0x00);
                }
            }

            // Keep the bus powered for the programming time
            ow_release();
            __delay_ms(10);

            ucResponse = ow_readByte();

            if (ucResponse != 0xAA) {
                bValid = false;
                //printf("ERROR 4\r\n");
                break;
            }
        }

        uPage++;
    } while ((uAddress + uPageSize < p_uAddress + p_uLength) && bValid);

#ifdef DEBUG
    if (!bValid) {
        printf("ow_writeMemory() error\r\n");
    }
#endif

    return bValid;
}
 */
/**
 * \brief Fonction pour lire un périphérique, importée du code original. Non utilisée.
 * @param p_pROM
 * @param p_uAddress
 * @param p_pData
 * @param p_uLength
 *//*
void ow_readMemory_old(ow_rom *p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength) {
    uint8_t aData[64];
    uint8_t uByte;
    uint8_t uReadCommand = 0x00;
    bool bPassword = false;
    bool bChecksum = false;
    bool bPower = false;
    uint8_t uPageSize;

    switch (p_pROM->familyCode) {
        case OW_FAMILY_DS1972:
        case OW_FAMILY_DS1973:
            uReadCommand = OW_COMMAND_READ_MEMORY;
            uPageSize = 144;

            // There is no checksum so we can break the read before the end
            if (p_uLength < uPageSize) {
                uPageSize = p_uLength;
            }
            break;
        case OW_FAMILY_DS1977:
            uReadCommand = OW_COMMAND_READ_MEMORY_WITH_PASSWORD;
            bPassword = true;
            bPower = true;
            bChecksum = true;
            uPageSize = 66;
            break;
    }

    ow_reset();
    ow_skipROM();
    ow_writeByte(uReadCommand);
    ow_writeByte(p_uAddress & 0xFF);
    ow_writeByte((p_uAddress >> 8) & 0xFF);

    if (bPassword) {
        // Supply the password
        for (uByte = 0; uByte < 8; uByte++) {
            ow_writeByte(0x00);
        }
    }

    if (bPower) {
        // Keep the bus powered for the reading time
        ow_release();
        __delay_ms(10);
    }

    // Read page including CRC
    for (uByte = 0; uByte < uPageSize; uByte++) {
        aData[uByte] = ow_readByte();
    }

    if (bChecksum) {
        // TODO : Verify checksum
    }

    // Copy required data
    memcpy(p_pData, aData, p_uLength);
}*/
/**
 * Fonction pour écrire le temps dans un RTC. Non utilisé.
 * @param p_pDateTime
 */ /*
void ow_writeClock(struct tm *p_pDateTime) {
    unsigned char ucDCB = 0x0C; // Enable the oscillator
    time_t stSeconds;

    stSeconds = mktime(p_pDateTime);

    ow_reset();
    ow_skipROM();

    ow_writeByte(OW_COMMAND_WRITE_CLOCK);
    ow_writeByte(ucDCB);
    ow_writeByte(stSeconds & 0xFF);
    ow_writeByte((stSeconds >> 8) & 0xFF);
    ow_writeByte((stSeconds >> 16) & 0xFF);
    ow_writeByte((stSeconds >> 24) & 0xFF);

    ow_reset();
}*/

/**
 * \brief Pour passer d'une forme à l'autre. N'est plus utilisé.
 * @param p_pROM
 * @param p_aArray
 * @param p_nOffset
 */ /*
void ow_romToArray(ow_rom *p_pROM, uint8_t *p_aArray, int p_nOffset) {
    p_aArray[p_nOffset + 0] = p_pROM->familyCode;

    for (uint8_t nByte = 0; nByte < 6; nByte++) {
        p_aArray[p_nOffset + nByte + 1] = p_pROM->serialNumber >> nByte * 8;
    }

    p_aArray[p_nOffset + 7] = p_pROM->crc;
}*/

/* ancienne version des commandes 1-wire provenant du code original.
 * N'est plus utilisé, pour réf. 
 * 
unsigned char macro_delay;

void drive_OW_low(void) {
    IO_1W_SetDigitalOutput();
    IO_1W_SetLow();
}

void drive_OW_high(void) {
    IO_1W_SetDigitalInput(); // laisser la sortie en haute impédance
    IO_1W_SetHigh();
}

unsigned char read_OW(void) {
    unsigned char read_data = 0;

    IO_1W_SetDigitalInput();

    if (HIGH == IO_1W_GetValue())
        read_data = SET;
    else
        read_data = CLEAR;

    return read_data;
}

unsigned char OW_reset_pulse(void)
 {
    unsigned char presence_detect;

    drive_OW_low(); // Drive the bus low
    __delay_us(480); // delay 480 microsecond (us)
    drive_OW_high(); // Release the bus
    __delay_us(70); // delay 70 microsecond (us)
    presence_detect = read_OW(); //Sample for presence pulse from slave
    __delay_us(410); // delay 410 microsecond (us)
    drive_OW_high(); // Release the bus

    return presence_detect;
}

void OW_write_bit(unsigned char write_bit) {
    if (write_bit) {
        //writing a bit '1'
        drive_OW_low(); // Drive the bus low
        __delay_us(6); // delay 6 microsecond (us)
        drive_OW_high(); // Release the bus
        __delay_us(64); // delay 64 microsecond (us)
    } else {
        //writing a bit '0'
        drive_OW_low(); // Drive the bus low
        __delay_us(60); // delay 60 microsecond (us)
        drive_OW_high(); // Release the bus
        __delay_us(10); // delay 10 microsecond for recovery (us)
    }
}

unsigned char OW_read_bit(void) {
    unsigned char read_data;
    //reading a bit 
    drive_OW_low(); // Drive the bus low
    __delay_us(6); // delay 6 microsecond (us)
    drive_OW_high(); // Release the bus
    __delay_us(9); // delay 9 microsecond (us)
    read_data = read_OW(); //Read the status of OW_PIN
    __delay_us(55); // delay 55 microsecond (us)	
    return read_data;
}
 

void ow_writeByte(uint8_t write_data) {
    unsigned char loop;

    for (loop = 0; loop < 8; loop++) {
        ow_writeBit(write_data & 0x01); //Sending LS-bit first
        write_data >>= 1; // shift the data byte for the next bit to send
    }
}

unsigned char OW_read_byte(void) {
    unsigned char loop, result = 0;

    for (loop = 0; loop < 8; loop++) {
        result >>= 1; // shift the result to get it ready for the next bit to receive
        if (ow_readBit())
            result |= 0x80; // if result is one, then set MS-bit
    }
    return result;
}
 */
