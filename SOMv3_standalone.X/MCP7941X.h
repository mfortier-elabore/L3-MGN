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

#ifdef XC8_TOOLCHAIN
#include <xc.h>
#include "mcc_generated_files/system/system.h"
#else
uint8_t fakeMemRTC[16];
uint8_t fakeMemEE[0xFF];

#define I2C_Write1ByteRegister(MCP7941X_RTC_ADDR, REG_ADDR, VAL)	do { fakeMemRTC[REG_ADDR] = VAL; } while (0)
#define I2C_Read1ByteRegister(MCP7941X_RTC_ADDR, REG_ADDR)		fakeMemRTC[REG_ADDR]
#define I2C_ReadDataBlock(MCP7941X_EE_ADDR, REG_ADDR, temp, size)	memcpy(temp, &fakeMemEE+REG_ADDR, size)
#endif



// RTC chip address
#define MCP7941X_RTC_ADDR           0x6F  // Address 1, RTC function
#define MCP7941X_EE_ADDR            0x57  // Address 2, EEPROM functions

// RTC registers
#define RTCC_SECONDS                0x00
#define RTCC_MINUTES                0x01
#define RTCC_HOUR                   0x02
#define RTCC_DAY                    0x03
#define RTCC_DATE                   0x04
#define RTCC_MONTH                  0x05
#define RTCC_YEAR                   0x06
#define CONTROL_REG                 0x07
#define CALIBRATION                 0x08

#define HEURE_MASQUE    0b00111111
#define SEC_MASQUE      0b01111111
#define MIN_MASQUE      0b01111111
#define JOUR_MASQUE     0b01111111
#define MOIS_MASQUE     0b00011111

// EEPROM registers
#define EUI64_NODE_ADDRESS          0xF0

// Function prototypes
void MCP7941X_setTime(struct tm * temps);
void MCP7941X_getTime(struct tm * t);

void getIdBase(uint8_t * idBase);
void setIdBase(void);

void MCP7941X_runTests(void);

uint8_t toBCD(uint8_t val_int);
uint8_t fromBCD(uint8_t raw);

#endif	/* MCP7941X_H */

