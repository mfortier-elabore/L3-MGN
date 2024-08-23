extern "C" {
#include "../../I2CHelper.h"
#include "../../MCP7941X.h"    
}

#include "CppUTest/TestHarness.h"
#include <stdio.h>

TEST_GROUP(I2CHelper) {
    /*
    void setup()
    {
      I2CHelper_Create();
    }

    void teardown()
    {
       I2CHelper_Destroy();
    }
     */

};

TEST(I2CHelper, I2CCorrectlyWritesDataToRegisterAddress) {
    uint8_t deviceAddress = 0x01;
    uint8_t registerAddress = 0x02;
    uint8_t data = 0xAA;

    I2CHelper_WriteRegister(deviceAddress, registerAddress, &data);

    CHECK_EQUAL(fakeI2CDevices[deviceAddress][registerAddress], data);
}

TEST(I2CHelper, I2CCorrectlyReadsDataFromRegisterAddress) {
    uint8_t deviceAddress = 0x01;
    uint8_t registerAddress = 0x02;
    uint8_t data;

    I2CHelper_ReadRegister(deviceAddress, registerAddress, &data);

    CHECK_EQUAL(fakeI2CDevices[deviceAddress][registerAddress], data);
}

TEST(I2CHelper, I2CCorrectlyReadsDataFromMultipleRegisters) {
    uint8_t deviceAddress = 0x01;
    uint8_t registerAddress = 0x02;
    uint8_t length = 8;
    uint8_t data[length];
    
    /*for(int i=0; i<0xF; ++i) {
        for(int j=0; j<0xF; ++j) {
            fakeI2CDevices[i][j] = 0;
        }^;
    }*/
    
    uint8_t writedata=0x55;
    
    I2CHelper_WriteRegister(deviceAddress, registerAddress, &writedata);
    I2CHelper_ReadMultipleRegisters(deviceAddress, registerAddress, &data[0], length);
    
    for(int i=0; i<length; ++i) {
        CHECK_EQUAL(fakeI2CDevices[deviceAddress][registerAddress+i], data[i]);
    }
}

TEST(I2CHelper, I2CCorrectlyWritesDataToMultipleRegisters) {
    uint8_t deviceAddress = MCP7941X_EE_ADDR;
    uint8_t registerAddress = EUI64_NODE_ADDRESS;
    uint8_t length = 8;
    uint8_t data[length] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
    
    for(int i=0; i<0xFF; ++i) {
        for(int j=0; j<0xFF; ++j) {
            fakeI2CDevices[i][j] = 0;
        };
    }
    
    I2CHelper_WriteMultipleRegisters(deviceAddress, registerAddress, &data[0], 8);
    
    //I2CHelper_ReadMultipleRegisters(deviceAddress, registerAddress, &data[0], length);
    
    //CHECK_TRUE(!memcmp(&fakeI2CDevices[deviceAddress][registerAddress], &data[0], 8));
    
    /*
    printf("Array output : ");    
    for(int j=0; j<length; j++) {
        printf("%i_%i - ", fakeI2CDevices[deviceAddress][registerAddress+j], data[j]);
    }
    */
    for(int j=0; j<length; j++) {
        CHECK_EQUAL(fakeI2CDevices[deviceAddress][registerAddress+j], data[j]);
    }
}