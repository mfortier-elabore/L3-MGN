/* 
 * File: 1-wire.h
 * Author: MFortier
 * Comments:
 * Revision history:
 *      Initial : 2023-04-03
 */

#ifndef ONE_WIRE_H
#define	ONE_WIRE_H

/********************************************
#define	SET		1
#define	CLEAR	0

void drive_one_wire_low(void);
void drive_one_wire_high(void);
unsigned char read__one_wire(void);
void OW_write_bit(unsigned char write_data);
unsigned char OW_read_bit(void);
unsigned char OW_reset_pulse(void);
void OW_write_byte(unsigned char write_data);
unsigned char OW_read_byte(void);

 *****************************************/

#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "CFH.h"

#ifdef XC8_TOOLCHAIN
#include "mcc_generated_files/system/system.h"
#else
#define IO_1W_SetDigitalInput()    do { fake1wPinDirection = 1; } while(0)
#define IO_1W_SetDigitalOutput()   do { fake1wPinDirection = 0; } while(0)
#define IO_1W_SetHigh()            do { fake1wPinValue = 1; } while(0)
#define IO_1W_SetLow()             do { fake1wPinValue = 0; } while(0)
#define IO_1W_GetValue()           fake1wPinValue
#define __delay_us(x)              do { } while(0);
#define __delay_ms(x)              do { } while(0);
uint8_t fake1wPinValue = 0;
uint8_t fake1wPinDirection = 0;
#endif

typedef struct {
    uint8_t familyCode;
    uint64_t serialNumber;
    uint8_t crc;
} ow_rom;

// ROM commands
#define OW_COMMAND_READ_ROM						0x33
#define OW_COMMAND_MATCH_ROM					0x55
#define OW_COMMAND_SKIP_ROM						0xCC
#define OW_COMMAND_SEARCH_ROM					0xF0

// DS1977 memory commands
#define OW_COMMAND_WRITE_SCRATCHPAD				0x0F
#define OW_COMMAND_READ_SCRATCHPAD				0xAA
#define OW_DS1977_COMMAND_COPY_SCRATCHPAD		0x99
#define OW_COMMAND_READ_MEMORY_WITH_PASSWORD	0x69

// DS1985 memory commands
#define OW_COMMAND_READ_MEMORY					0xF0

#define OW_DS1972_COMMAND_COPY_SCRATCHPAD		0x55

// DS1904 clock commands
#define OW_COMMAND_READ_CLOCK		0x66
#define OW_COMMAND_WRITE_CLOCK		0x99

#define OW_FAMILY_DS1904			0x24	// RTC

#define OW_FAMILY_DS1972			0x2D	// 128B EEPROM
#define OW_FAMILY_DS2431			0x2D	// 128B EEPROM
#define OW_FAMILY_DS1973			0x23	// 512B EEPROM
#define OW_FAMILY_DS1977			0x37	// 32KB EEPROM
#define OW_FAMILY_DS1985			0x0B	// 2KB EEPROM
#define OW_FAMILY_INVALID           0x00    // Code invalide ou inconnu

#define NB_ESSAIS_ECRITURE_MAX      0x0A    // 10 essais d'écriture permis
#define NB_ESSAIS_LECTURE_MAX       0x0A

// Fonctions niveau bus 1-wire
void ow_release(void);
void ow_drive(void);
bool ow_reset(void);
bool read_OW(void);

// Fonctions niveau data
void ow_writeBit(bool p_bValue);
bool ow_readBit(void);
void ow_writeByte(uint8_t p_ucValue);
bool ow_write8bytes(uint16_t p_uAddress, uint8_t * p_pData);
uint8_t ow_readByte(void);
uint8_t do_crc(uint8_t * address,  uint8_t p_uLength);

// Fonctions niveau application
void ow_readROM(uint8_t * array);
bool ow_readROM_safe(uint8_t * array);
void ow_skipROM(void);
uint8_t crc8(uint8_t *addr, uint8_t len);
bool ow_writeMemory(uint8_t * p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength);
void ow_readMemory(uint8_t * p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength);
void ow_readMemory_Safe(uint8_t * p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength);
bool ow_readClock(struct tm *p_pDateTime);

// Fonctions désuettes
//void ow_readROM_old(ow_rom *p_pROM);
//void ow_matchROM(ow_rom *p_pROM);
//int ow_searchROM(ow_rom *p_aROMs);
//bool ow_testCRC(ow_rom *p_pROM);
//bool ow_writeMemory_old(ow_rom *p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength);
//void ow_readMemory_old(ow_rom *p_pROM, uint16_t p_uAddress, void *p_pData, uint8_t p_uLength);
//void ow_writeClock(struct tm *p_pDateTime);
//void ow_romToArray(ow_rom *p_pROM, uint8_t *p_aArray, int p_nOffset);

#endif	/* ONE_WIRE_H */
