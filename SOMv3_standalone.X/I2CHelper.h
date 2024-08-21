/*
 * File:   I2CHelper.h
 * Author: marcf
 *
 * Created on 2024-08-20
 */

#ifndef D_I2CHelper_H
#define D_I2CHelper_H

#include <stdint.h>
#include <stdbool.h>

#ifdef XC8_TOOLCHAIN
#include "mcc_generated_files/i2c_host/i2c_host_interface.h"
#include "mcc_generated_files/system/system.h"
#else
#include <string.h>
extern uint8_t fakeI2CDevices[0xFF][0xFF];
extern uint8_t selectedFakeI2CDevice;
extern uint8_t selectedFakeI2CDeviceRegister;
extern uint8_t statut;
#endif


void I2CHelper_WriteRegister(uint8_t deviceAddress, uint8_t registerAddress, uint8_t * data);
void I2CHelper_ReadRegister(uint8_t deviceAddress, uint8_t registerAddress, uint8_t * data);
void I2CHelper_ReadMultipleRegisters(uint8_t deviceAddress, uint8_t registerAddress, uint8_t * data, size_t length);
void I2CHelper_WriteMultipleRegisters(uint8_t deviceAddress, uint8_t registerAddress, uint8_t * data, size_t length);


#endif  /* D_FakeI2CHelper_H */
