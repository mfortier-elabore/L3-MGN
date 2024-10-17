/* 
 * File: MCP7941X.h
 * Author: MFortier
 * Comments:
 * Revision history:
 *      Initial : 2023-04-03
 *      Rev1 : 2024-08-20 - Modifications pour L3-MGN
 */

#ifndef MCP7941X_H
#define	MCP7941X_H

#include <time.h>
#include "CFH.h"
#include "I2CHelper.h"

#ifdef XC8_TOOLCHAIN
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/i2c_host/i2c_host_interface.h"
#include "mcc_generated_files/nvm/nvm.h"
#else
extern uint8_t fakeEEMem[0xFF];

typedef uint8_t eeprom_data_t;
typedef uint8_t eeprom_address_t;

eeprom_data_t EEPROM_Read(eeprom_address_t address);
void EEPROM_Write(eeprom_address_t address, eeprom_data_t p_data);
#endif



// RTC chip address
#define MCP7941X_RTC_ADDR           0x6F  // Address 1, RTC function
#define MCP7941X_EE_ADDR            0x57  // Address 2, EEPROM functions

// RTC registers
// Doivent etre des const uint8_t a cause de l'appel I2C
const uint8_t RTCC_SECONDS = 0x00;
const uint8_t RTCC_MINUTES = 0x01;
const uint8_t RTCC_HOUR = 0x02;
const uint8_t RTCC_DAY = 0x03;
const uint8_t RTCC_DATE = 0x04;
const uint8_t RTCC_MONTH = 0x05;
const uint8_t RTCC_YEAR = 0x06;
const uint8_t CONTROL_REG = 0x07;
const uint8_t CALIBRATION = 0x08;

#define HEURE_MASQUE    0b00111111
#define SEC_MASQUE      0b01111111
#define MIN_MASQUE      0b01111111
#define JOUR_MASQUE     0b01111111
#define MOIS_MASQUE     0b00011111

// EEPROM registers
const uint8_t EUI64_NODE_ADDRESS = 0xF0;
//const uint8_t EEUNLOCK_ADDRESS = 0x09;    // Pour changer mémoire protégée

// Function prototypes
void MCP7941X_setTime(struct tm * temps);
void MCP7941X_getTime(struct tm * t);

void getIdBase(uint8_t * idBase);
void setIdBase(void);

void MCP7941X_runTests(void);

uint8_t toBCD(uint8_t val_int);
uint8_t fromBCD(uint8_t raw);

#endif	/* MCP7941X_H */

