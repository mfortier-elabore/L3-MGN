#include "I2CHelper.h"

#ifdef TDD_SIM
uint8_t fakeI2CDevices[0x02][0xFF];
uint8_t selectedFakeI2CDevice = 0;
uint8_t selectedFakeI2CDeviceRegister = 0;

enum {
    WAITING_REGISTER = 1, WAITING_DATA = 2
};
uint8_t statut = WAITING_REGISTER;

static bool I2C1_Write(uint16_t address, uint8_t *data, size_t dataLength) {
    switch (statut) {
        case WAITING_REGISTER:
            selectedFakeI2CDevice = (uint8_t) address;
            selectedFakeI2CDeviceRegister = *data;
            statut = WAITING_DATA;
            break;
        case WAITING_DATA:
            memcpy(&fakeI2CDevices[selectedFakeI2CDevice][selectedFakeI2CDeviceRegister], data, dataLength);
            statut = WAITING_REGISTER;
            selectedFakeI2CDevice = 0;
            selectedFakeI2CDeviceRegister = 0;
            break;
        default:
            break;
    }
    return 0;
}

static bool I2C1_WriteRead(uint16_t address, uint8_t *writeData, size_t writeLength, uint8_t *readData, size_t readLength) {

    for (int i = 0; i < readLength; ++i) {
        selectedFakeI2CDevice = (uint8_t) address;
        selectedFakeI2CDeviceRegister = *writeData;

        //*readData[i] = fakeI2CDevices[selectedFakeI2CDevice][selectedFakeI2CDeviceRegister+i];

        memcpy(readData + i, &fakeI2CDevices[selectedFakeI2CDevice][selectedFakeI2CDeviceRegister + i], 1);

        selectedFakeI2CDevice = 0;
        selectedFakeI2CDeviceRegister = 0;
    }
    return 0;
}

typedef struct {
    bool(*Write)(uint16_t address, uint8_t *data, size_t dataLength);
    bool(*WriteRead)(uint16_t address, uint8_t *writeData, size_t writeLength, uint8_t *readData, size_t readLength);
} I2C1_Host_t;

static I2C1_Host_t I2C1_Host = {
    .Write = I2C1_Write,
    .WriteRead = I2C1_WriteRead
};

void I2CHelper_Create(void) {
}

void I2CHelper_Destroy(void) {
}
#endif

void I2CHelper_WriteRegister(uint8_t deviceAddress, uint8_t registerAddress, uint8_t * data) {
    I2C1_Host.Write(deviceAddress, &registerAddress, 1);
    I2C1_Host.Write(deviceAddress, data, 1);
}

void I2CHelper_ReadRegister(uint8_t deviceAddress, uint8_t registerAddress, uint8_t * data) {
    I2C1_Host.WriteRead(deviceAddress, &registerAddress, 1, data, 1);
}

void I2CHelper_ReadMultipleRegisters(uint8_t deviceAddress, uint8_t registerAddress, uint8_t * data, size_t length) {
    I2C1_Host.WriteRead(deviceAddress, &registerAddress, 1, data, length);
}

void I2CHelper_WriteMultipleRegisters(uint8_t deviceAddress, uint8_t registerAddress, uint8_t * data, size_t length) {
    I2C1_Host.Write(deviceAddress, &registerAddress, 1);
    I2C1_Host.Write(deviceAddress, data, length);
}