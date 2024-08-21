extern "C" {
#include "../../MCP7941X.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(MCP7941X) {
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

TEST(MCP7941X, ToBCDWorks) {
    CHECK_EQUAL(toBCD(10),16);
}

TEST(MCP7941X, fromBCDWorks) {
    CHECK_EQUAL(fromBCD(16),10);
}

TEST(MCP7941X, setIdBaseWorks) {
    // clear fakeEEMem
    for(int i=0; i<0xFF; ++i) {
        fakeEEMem[i] = 0;
    }
    
    // ecrire adresse bidon dans le fake I2C
    uint8_t fake1WAddress[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
    I2CHelper_WriteMultipleRegisters(MCP7941X_EE_ADDR, EUI64_NODE_ADDRESS, &fake1WAddress[0], 8 * sizeof (uint8_t));
    
    setIdBase();
    
    CHECK_TRUE(!memcmp(&fake1WAddress[0], &fakeEEMem[MEMORY_ADDRESS_BASE_SERIAL], 8));
}

TEST(MCP7941X, getIdBaseWorks) {
    // clear fakeEEMem
    for(int i=0; i<0xFF; ++i) {
        fakeEEMem[i] = 0;
    }
    
    // ecrire adresse bidon dans le fake I2C
    uint8_t fake1WAddress[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
    uint8_t readAddr[8] = { 0 };
    
    I2CHelper_WriteMultipleRegisters(MCP7941X_EE_ADDR, EUI64_NODE_ADDRESS, &fake1WAddress[0], 8 * sizeof (uint8_t));
    
    setIdBase();
    
    getIdBase(&readAddr[0]);
    
    CHECK_TRUE(!memcmp(&fake1WAddress[0], &readAddr[0], 8));
    
    readAddr[3] = 0x3;
    
    CHECK_FALSE(!memcmp(&fake1WAddress[0], &readAddr[0], 8));
}

TEST(MCP7941X, GetAndSetTimeWorks) {
    struct tm temps;
    struct tm temps2;
    
    int64_t temps_epoch = 0;
    int64_t temps2_epoch = 0;
    
    temps.tm_mday = 28;
    temps.tm_mon = 7;
    temps.tm_year = 1+100; // 1 = 2001
    temps.tm_hour = 15;
    temps.tm_min = 25;
    temps.tm_sec = 0;
    
    MCP7941X_setTime(&temps);
    MCP7941X_getTime(&temps2);
    
    
    temps_epoch=mktime(&temps);
    temps2_epoch=mktime(&temps2);
    
    /*char buffer[26];
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S\n", &temps);
    puts(buffer);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S\n", &temps2);
    puts(buffer);*/
    
    CHECK_EQUAL(temps_epoch, temps2_epoch);
    
    temps.tm_mday = 25;
    temps_epoch=mktime(&temps);
    
    CHECK_FALSE(temps_epoch == temps2_epoch);
}