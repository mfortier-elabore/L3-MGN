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
#define LIS2DTW12_WHO_AM_I      0x0F    // Registre WHOAMI, retourne 01000100 -> 0x44
#define LIS2DTW12_CTRL1         0x20    // Registres de controle
#define LIS2DTW12_CTRL2         0x21    // Registres de controle
#define LIS2DTW12_CTRL3         0x22    // Registres de controle
#define LIS2DTW12_CTRL4         0x23    // Registres de controle
#define LIS2DTW12_CTRL5         0x24    // Registres de controle
#define LIS2DTW12_CTRL6         0x25    // Registres de controle

#define LIS2DTW12_CTRL6         0x25    // Registres de controle


#ifdef XC8_TOOLCHAIN
#include <xc.h>
#include <stdint.h>
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/i2c_host/i2c_host_interface.h"
#else
#endif

void LIS2DTW12_Create(void);
void LIS2DTW12_Destroy(void);

#endif  /* D_FakeLIS2DTW12_H */
