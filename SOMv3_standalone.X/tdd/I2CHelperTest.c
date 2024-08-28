#ifdef TDD_SIM

#include "AllTests.h"
#include "../I2CHelper.h"

/*void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}*/

void I2CHelper_I2CCorrectlyWritesDataToRegisterAddress(void) {
    uint8_t deviceAddress = 0x01;
    uint8_t registerAddress = 0x02;
    uint8_t data = 0xAA;

    I2CHelper_WriteRegister(deviceAddress, registerAddress, &data);

    TEST_ASSERT_EQUAL(fakeI2CDevices[deviceAddress][registerAddress], data);
}

void I2CHelper_I2CCorrectlyReadsDataFromRegisterAddress(void) {
    uint8_t deviceAddress = 0x01;
    uint8_t registerAddress = 0x02;
    uint8_t data;

    I2CHelper_ReadRegister(deviceAddress, registerAddress, &data);

    TEST_ASSERT_EQUAL(fakeI2CDevices[deviceAddress][registerAddress], data);
}

void I2CHelper_I2CCorrectlyReadsDataFromMultipleRegisters(void) {
    uint8_t deviceAddress = 0x01;
    uint8_t registerAddress = 0x02;
    uint8_t length = 8;
    uint8_t data[8];
    
    /*for(int i=0; i<0xF; ++i) {
        for(int j=0; j<0xF; ++j) {
            fakeI2CDevices[i][j] = 0;
        }^;
    }*/
    
    uint8_t writedata=0x55;
    
    I2CHelper_WriteRegister(deviceAddress, registerAddress, &writedata);
    I2CHelper_ReadMultipleRegisters(deviceAddress, registerAddress, &data[0], length);
    
    for(int i=0; i<length; ++i) {
        TEST_ASSERT_EQUAL(fakeI2CDevices[deviceAddress][registerAddress+i], data[i]);
    }
}

void I2CHelper_I2CCorrectlyWritesDataToMultipleRegisters(void) {
    uint8_t deviceAddress = 0x02;
    uint8_t registerAddress = 0x03;
    uint8_t length = 8;
    uint8_t data[] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
    
    for(int i=0; i<0x02; ++i) {
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
        TEST_ASSERT_EQUAL(fakeI2CDevices[deviceAddress][registerAddress+j], data[j]);
    }
}

void RUN_I2CHELPER_TESTS(void) {
    RUN_TEST(I2CHelper_I2CCorrectlyWritesDataToRegisterAddress);
    RUN_TEST(I2CHelper_I2CCorrectlyReadsDataFromRegisterAddress);
    RUN_TEST(I2CHelper_I2CCorrectlyReadsDataFromMultipleRegisters);
    RUN_TEST(I2CHelper_I2CCorrectlyWritesDataToMultipleRegisters);
}

#endif