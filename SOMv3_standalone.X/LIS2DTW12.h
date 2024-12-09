/* 
 * File: LIS2DTW12.h
 * Author: MFortier
 * Comments:
 * Revision history:
 *      Initial : 2024-08-22
 */

#ifndef D_LIS2DTW12_H
#define D_LIS2DTW12_H

#include "I2CHelper.h"

// Accelerometer chip address
#define LIS2DTW12_ADDR          0x19    // 001100x | 1 -> 0x19

// Registres
const uint8_t LIS2DTW12_WHO_AM_I = 0x0F; // Registre WHOAMI, retourne 01000100 -> 0x44
const uint8_t LIS2DTW12_CTRL1 = 0x20; // CTRL config fonctionnement
const uint8_t LIS2DTW12_CTRL2 = 0x21; // CTRL Boot et IOs
const uint8_t LIS2DTW12_CTRL3 = 0x22; // CTRL pour les interrupts
const uint8_t LIS2DTW12_CTRL4 = 0x23; // CTRL interrupts pour tap et FIFO
const uint8_t LIS2DTW12_CTRL5 = 0x24; // CTRL interrupts sleep et fifo
const uint8_t LIS2DTW12_CTRL6 = 0x25; // CTRL Bandwidth et filtres
const uint8_t LIS2DTW12_FIFO_CTRL = 0x2E; // CTRL FIFO. Bypass : 0x00

const uint8_t LIS2DTW12_OUT_X_L = 0x28; // Byte faible accel X
const uint8_t LIS2DTW12_OUT_X_H = 0x29; // Byte fort accel X
const uint8_t LIS2DTW12_OUT_Z_L = 0x2C; // Byte faible accel z
const uint8_t LIS2DTW12_OUT_Z_H = 0x2D; // Byte fort accel z

#ifdef TDD_SOFTWARE
#else
#include <xc.h>
#include <stdint.h>
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/i2c_host/i2c_host_interface.h"
#endif

typedef struct Accel_t {
    uint16_t slowMovingAverage; // To establish baseline
    uint16_t fastMovingAverage; // to smoothen current reading
    uint16_t lastValue;
    uint16_t filteredValue;
} Accel_t;

void LIS2DTW12_Create(void);
void LIS2DTW12_Destroy(void);
int16_t LIS2DTW12_ReadZAxis(void);
void LIS2DTW12_Update(void);
uint16_t LIS2DTW12_GetValue(void);

#if defined(TDD_SOFTWARE) || defined(TDD_HARDWARE)
extern struct Accel_t accel;
#endif

#endif  /* LIS2DTW12_H */
