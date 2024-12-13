/*
 * File:   MCP7941X.c
 * Author: marcf
 *
 * Created on 1 avril 2023, 09:11
 *     Rev1 : 2024-08-20 - Modifications pour L3-MGN
 */
#include "MCP7941X.h"

#ifdef TDD_SOFTWARE
uint8_t fakeEEMem[0xFF];

eeprom_data_t EEPROM_Read(eeprom_address_t address) {
    return fakeEEMem[address];
}

void EEPROM_Write(eeprom_address_t address, eeprom_data_t data) {
    fakeEEMem[address] = data;
}
#endif

/**
 * \brief Fonction pour écrire le temps dans le RTC local
 * @param temps
 */
void MCP7941X_setTime(struct tm * temps) {
    // Enable -> VBATEN
    uint8_t data = 0x08;
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &RTCC_DAY, &data);

    // ST -> start OSC
    data = 0x80;
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &RTCC_SECONDS, &data);

    // Format 24h
    data = 0x01;
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &RTCC_HOUR, &data);

    // Config sortie 1Hz
    data = 0x40;
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &CONTROL_REG, &data);
    __delay_ms(100);

    uint8_t min_raw = toBCD((uint8_t)temps->tm_min) & MIN_MASQUE;
    uint8_t heure_raw = toBCD((uint8_t)temps->tm_hour) & HEURE_MASQUE; // GMT-4
    uint8_t jour_raw = toBCD((uint8_t)temps->tm_mday) & JOUR_MASQUE;
    uint8_t mois_raw = toBCD((uint8_t)temps->tm_mon)& MOIS_MASQUE;
    uint8_t an_raw = toBCD((uint8_t)temps->tm_year - 100);
    
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &RTCC_DATE, &jour_raw);
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &RTCC_MONTH, &mois_raw);
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &RTCC_YEAR, &an_raw);
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &RTCC_HOUR, &heure_raw);
    I2CHelper_WriteRegister(MCP7941X_RTC_ADDR, &RTCC_MINUTES, &min_raw);
}

/**
 * \brief Utilitaire de conversion hexa -> bcd
 * @param val_int
 * @return 
 */
uint8_t toBCD(uint8_t val_int) {
    return (uint8_t)((val_int / 10) << 4) | (val_int % 10);
}

/**
 * \brief Utilitaire de conversion bcd-> hexa
 * @param raw
 * @return 
 */
uint8_t fromBCD(uint8_t raw) {
    return (raw & 0x0F) + (uint8_t)(10 * (raw >> 4));
}

void MCP7941X_getTime(struct tm * t) {
    uint8_t jour_raw, mois_raw, an_raw, heure_raw, min_raw, sec_raw;

    // lecture I2C
    I2CHelper_ReadRegister(MCP7941X_RTC_ADDR, &RTCC_DATE, &jour_raw);
    jour_raw = jour_raw & JOUR_MASQUE;
    
    I2CHelper_ReadRegister(MCP7941X_RTC_ADDR, &RTCC_MONTH, &mois_raw);
    mois_raw = mois_raw & MOIS_MASQUE;
    
    I2CHelper_ReadRegister(MCP7941X_RTC_ADDR, &RTCC_YEAR, &an_raw);
    
    I2CHelper_ReadRegister(MCP7941X_RTC_ADDR, &RTCC_SECONDS, &sec_raw);
    sec_raw = sec_raw & SEC_MASQUE;
    
    I2CHelper_ReadRegister(MCP7941X_RTC_ADDR, &RTCC_MINUTES, &min_raw);
    min_raw = min_raw & MIN_MASQUE;
    
    I2CHelper_ReadRegister(MCP7941X_RTC_ADDR, &RTCC_HOUR, &heure_raw);
    heure_raw = heure_raw & HEURE_MASQUE;
    
    // Convertir du BDC et met à jour la struct globale
    t->tm_mday = fromBCD(jour_raw);
    t->tm_mon = fromBCD(mois_raw);
    t->tm_year = fromBCD(an_raw) + 100; // Compense pour les années 2000 - 2099
    t->tm_hour = fromBCD(heure_raw);
    t->tm_min = fromBCD(min_raw);
    t->tm_sec = fromBCD(sec_raw);
    
    printf("Date / Heure : %i/%i/%i %i:%i:%i\n", t->tm_mday, t->tm_mon+1, t->tm_year-100+2000, t->tm_hour-4, t->tm_min, t->tm_sec);
}

/**
 * \brief Lire serial de la base dans le EEPROM local
 * @param idBase variable qui contiendra le SN de la base
 */
void getIdBase(uint8_t * idBase) {
    uint8_t byte;
    
    printf("ID base memorisee : ");
    for (uint8_t i = 0; i < 8; ++i) {
        byte = EEPROM_Read(MEMORY_ADDRESS_BASE_SERIAL + i);
        idBase[i] = byte;
        printf("%x", byte & 0xff);
    }
    printf("\n");
}

/**
 * \brief Écrire le serial de la base dans le EEPROM. Uniquement la première fois!
 */
void setIdBase() {

    // Le ID provient de l'adresse unique du RTC
    uint8_t temp[8] = {0};
    
    // Node address en const int, car sinon l'adresse ne vit pas assez longtemps
    // Et la valeur lue est ffffffffffffffff
    I2CHelper_ReadMultipleRegisters(MCP7941X_EE_ADDR, &EUI64_NODE_ADDRESS, &temp[0], 8 * sizeof (uint8_t));

    uint8_t id[8] = {0};
    getIdBase(id);
    if (!memcmp(&temp, &id, 8 * sizeof (uint8_t))) {
        // ID déjà inscrit, on skippe
        printf("ID en memoire correct.\n");
        return;
    }

    printf("ID base lue : ");
    // Écriture des 8 bytes de l'adresse
    for (uint8_t i = 0; i < 8; ++i) {
        printf("%x", temp[i] & 0xff);
        EEPROM_Write(MEMORY_ADDRESS_BASE_SERIAL + i, temp[i]);
    }
    printf("\n");
}