/* 
 * File: cartouche.h
 * Author: MFortier
 * Comments:
 * Revision history:
 *      Initial : 2023-04-03
 */

#ifndef CARTOUCHE_H
#define	CARTOUCHE_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#ifdef XC8_TOOLCHAIN
#include <xc.h>
#include "mcc_generated_files/system/system.h"
#else // Définitions pour les tests unitaires
uint8_t fakeMem[16];
#define DATAEE_WriteByte(x, y)	do { fakeMem[x] = y; } while(0);
#define DATAEE_ReadByte(x)	fakeMem[x];
#endif


#include "1-wire.h"
#include "MCP7941X.h"
#include "leds.h"

// Configuration
#define NB_MINUTES_ATTENTE          0x01  // 3C = 60 minutes

#define MODEL						0x02  // était 0x02
#define HARDWARE_REVISION			0x11  // était 0x10
#define SOFTWARE_REVISION			0x11  // était 0x10

// Adresses eeprom local
#define MEMORY_ADDRESS_LAST_CARTRIDGE_SERIAL	0x0000
#define MEMORY_ADDRESS_BASE_SERIAL              0x0008

// Adresses dans le iButton
#define CARTRIDGE_ADDRESS_MODE						0x0000	///> Define what to do with this iButton (configuration or not)
#define CARTRIDGE_WRITES_COUNT						0x0001	///>
#define CARTRIDGE_WRITES_COUNT2						0x0050	///> 
#define CARTRIDGE_ADDRESS_HOST_MODEL				0x0008	///> Model of the host base
#define CARTRIDGE_ADDRESS_HOST_HARDWARE_REVISION	0x0009	///> Hardware revision of the host base
#define CARTRIDGE_ADDRESS_HOST_SOFTWARE_REVISION	0x000A	///> Software revision of the host base
#define CARTRIDGE_ADDRESS_HOST_SERIAL				0x000B	///> Serial of the host base
#define CARTRIDGE_ADDRESS_HOST_SERIAL2				0x0040	///> Serial of the host base

#define CARTRIDGE_ADDRESS_DATE_INSTALLED			0x0018	///> Installation date of the cartridge
#define CARTRIDGE_ADDRESS_DATE_INSTALLED2			0x0048	///> Installation date of the cartridge
#define CARTRIDGE_ADDRESS_DATE_REMOVED				0x001C	///> Removal date of the cartridge
#define CARTRIDGE_ADDRESS_DATE_REMOVED2				0x0054	///> Removal date of the cartridge

// Adresses des crc
#define CARTRIDGE_ADDRESS_HOST_SERIAL_CRC           0x004C  ///> CRC for IUB
#define CARTRIDGE_ADDRESS_DATE_INSTALLED_CRC		0x004D	///> CRC for Installed Date
#define CARTRIDGE_WRITES_COUNT_CRC                  0x0058  ///> CRC for Writes Count
#define CARTRIDGE_ADDRESS_DATE_REMOVED_CRC          0x0059  ///> CRC for Date Removed

// Inutilisés
#define CARTRIDGE_ADDRESS_OPERATION_TIME			0x0020	///> Duration of operation of the filtration (sec)
#define CARTRIDGE_ADDRESS_MAXIMUM_OPERATION_TIME	0x0024	///> Duration maximum of operation of the filtration (sec)

// Modes de cartouches. Inutilisé : toujours 0x00
#define CARTRIDGE_MODE_CARTRIDGE					0x00
#define CARTRIDGE_MODE_CONFIGURE					0x01
#define CARTRIDGE_MODE_CARTRIDGE_CONFIGURE			0x02

// Pour la routine qui fait le polling du 1-wire
extern bool update1W;

// Cartouches et base
void TMR0_2s_ISR(void);
void pollingCartouche(void);

void getCartoucheInstallee(uint8_t * cartouche);
void setCartoucheInstallee(uint8_t * pCartouche);

bool cartoucheDejaInstallee(uint8_t * nouvelleCartouche);

void executerCartoucheROM(uint8_t * nouvelleCartouche);
bool executerCartoucheRTC(void);

bool ecrireDateRetrait(uint8_t *p_pCartridge);
void ecrireNouvelleCartouche(void);
uint32_t lireDateInstallee(uint8_t * cartouche);
void lireIUBcartouche(uint8_t * p_pCartridge, uint8_t * array);

// Utilitaires
uint32_t flip(uint32_t p_uValue);
void CFH_runTests(void);
void CFH_eraseROM(void);

// Fonctions désuettes
//void readMaximumOperationTime(ow_rom *p_pLocal, uint32_t *p_pMaximumOperationTime);
//void writeMaximumOperationTime(ow_rom *p_pLocal, uint32_t *p_pMaximumOperationTime);
//void readOperationTime(ow_rom *p_pLocal, uint32_t *p_pOperationTime);
//void writeOperationTime(ow_rom *p_pLocal, uint32_t *p_pOperationTime);

#endif	/* CARTOUCHE_H */

