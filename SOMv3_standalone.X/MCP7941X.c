/*
 * File:   MCP7941X.c
 * Author: marcf
 *
 * Created on 1 avril 2023, 09:11
 *     Rev1 : 2024-08-20 - Modifications pour L3-MGN
 */
#include "MCP7941X.h"

/**
 * \brief Fonction pour écrire le temps dans le RTC local
 * @param temps
 */
void MCP7941X_setTime(struct tm * temps) {
    // Enable -> VBATEN
    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, RTCC_DAY, 0x08);

    // ST -> start OSC
    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, RTCC_SECONDS, 0x80);

    // Format 24h
    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, RTCC_HOUR, 0x01);

    // Config sortie 1Hz
    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, CONTROL_REG, 0x40);
    __delay_ms(100);

    uint8_t min_raw = toBCD(temps->tm_min) & MIN_MASQUE;
    uint8_t heure_raw = toBCD(temps->tm_hour) & HEURE_MASQUE; // GMT-4
    uint8_t jour_raw = toBCD(temps->tm_mday) & JOUR_MASQUE;
    uint8_t mois_raw = toBCD(temps->tm_mon)& MOIS_MASQUE;
    uint8_t an_raw = toBCD(temps->tm_year - 100);

    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, RTCC_DATE, jour_raw);
    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, RTCC_MONTH, mois_raw);
    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, RTCC_YEAR, an_raw);
    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, RTCC_HOUR, heure_raw);
    I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, RTCC_MINUTES, min_raw);
}

/**
 * \brief Utilitaire de conversion hexa -> bcd
 * @param val_int
 * @return 
 */
uint8_t toBCD(uint8_t val_int) {
    return ((val_int / 10) << 4) | (val_int % 10);
}

/**
 * \brief Utilitaire de conversion bcd-> hexa
 * @param raw
 * @return 
 */
uint8_t fromBCD(uint8_t raw) {
    return (raw & 0x0F) + 10 * (raw >> 4);
}

void MCP7941X_getTime(struct tm * t) {
    uint8_t jour_raw, mois_raw, an_raw, heure_raw, min_raw, sec_raw;

    // lecture I2C
    jour_raw = I2C_Read1ByteRegister(MCP7941X_RTC_ADDR, RTCC_DATE);
    jour_raw = jour_raw & JOUR_MASQUE;
    mois_raw = I2C_Read1ByteRegister(MCP7941X_RTC_ADDR, RTCC_MONTH);
    mois_raw = mois_raw & MOIS_MASQUE;
    an_raw = I2C_Read1ByteRegister(MCP7941X_RTC_ADDR, RTCC_YEAR);
    sec_raw = I2C_Read1ByteRegister(MCP7941X_RTC_ADDR, RTCC_SECONDS);
    sec_raw = sec_raw & SEC_MASQUE;
    min_raw = I2C_Read1ByteRegister(MCP7941X_RTC_ADDR, RTCC_MINUTES);
    min_raw = min_raw & MIN_MASQUE;
    heure_raw = I2C_Read1ByteRegister(MCP7941X_RTC_ADDR, RTCC_HOUR);
    heure_raw = heure_raw & HEURE_MASQUE;

    // Convertir du BDC et met à jour la struct globale
    t->tm_mday = fromBCD(jour_raw);
    t->tm_mon = fromBCD(mois_raw);
    t->tm_year = fromBCD(an_raw) + 100; // Compense pour les années 2000 - 2099
    t->tm_hour = fromBCD(heure_raw);
    t->tm_min = fromBCD(min_raw);
    t->tm_sec = fromBCD(sec_raw);
}

/**
 * \brief Lire serial de la base dans le EEPROM local
 * @param idBase variable qui contiendra le SN de la base
 */
void getIdBase(uint8_t * idBase) {
    uint8_t byte;
    for (uint8_t i = 0; i < 8; ++i) {
        byte = DATAEE_ReadByte(MEMORY_ADDRESS_BASE_SERIAL + i);
        idBase[i] = byte;
    }
}

/**
 * \brief Écrire le serial de la base dans le EEPROM. Uniquement la première fois!
 */
void setIdBase() {

    // Le ID provient de l'adresse unique du RTC
    uint8_t temp[8];
    I2C_ReadDataBlock(MCP7941X_EE_ADDR, 0xF0, temp, 8 * sizeof (uint8_t));

    uint8_t id[8] = {0};
    getIdBase(id);
    if (!memcmp(&temp, &id, 8 * sizeof (uint8_t))) {
        // ID déjà inscrit, on skippe
        return;
    }

    // Écriture des 8 bytes de l'adresse
    for (uint8_t i = 0; i < 8; ++i) {
        DATAEE_WriteByte(MEMORY_ADDRESS_BASE_SERIAL + i, temp[i]);
    }
}

/*
void MCP7941X_runTests(void) {
    uint8_t id[8];

    //MCP7941X_getID(&id);

    setIdBase();

    uint8_t jour, mois, an;
    uint8_t heure, min, sec;

    MCP7941X_get_date(&jour, &mois, &an);

    MCP7941X_get_time(&heure, &min, &sec);

    if (an == 0x80) {
        MCP7941X_set_date_time(9, 4, 23, 16, 10);
    }

    uint32_t epoch = MCP7941X_getTime();

}*/