#if defined(TDD_SOFTWARE) || defined(TDD_HARDWARE)

#include "AllTests.h"
#include "../MCP7941X.h"

/*void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}*/

void MCP7941X_ToBCDWorks(void) {
    TEST_ASSERT_EQUAL(toBCD(10), 16);
}

void MCP7941X_fromBCDWorks(void) {
    TEST_ASSERT_EQUAL(fromBCD(16), 10);
}

void MCP7941X_setIdBaseWorks(void) {
    uint8_t length = 8;
    uint8_t readData[8] = {0};
    uint8_t byte;
    uint8_t zeros[8] = {0};
    
    // clear EEM
    for (uint8_t i = 0; i < length; ++i) {
        EEPROM_Write(MEMORY_ADDRESS_BASE_SERIAL + i, 0);
    }
    
    TEST_ASSERT_TRUE(!memcmp(&zeros[0], &readData[0], length));

#ifdef TDD_SOFTWARE
    // ecrire adresse bidon dans le fake I2C
    uint8_t fake1WAddress[] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
    I2CHelper_WriteMultipleRegisters(MCP7941X_EE_ADDR, EUI64_NODE_ADDRESS, &fake1WAddress[0], 8 * sizeof (uint8_t));
#endif
    
    setIdBase();
    
    for (uint8_t i = 0; i < length; ++i) {
        byte = EEPROM_Read(MEMORY_ADDRESS_BASE_SERIAL + i);
        readData[i] = byte;
    }

#ifdef TDD_SOFTWARE
    TEST_ASSERT_TRUE(!memcmp(&fake1WAddress[0], &readData[0], length));
#else
    TEST_ASSERT_FALSE(!memcmp(&zeros[0], &readData[0], length));
#endif
}

void MCP7941X_getIdBaseWorks(void) {
    // clear fakeEEMem
    /*for(int i=0; i<0xFF; ++i) {
        fakeEEMem[i] = 0;
    }*/

    // ecrire adresse bidon dans le fake I2C
    uint8_t fake1WAddress[8] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
    uint8_t readAddr[8] = {0};

    I2CHelper_WriteMultipleRegisters(MCP7941X_EE_ADDR, EUI64_NODE_ADDRESS, &fake1WAddress[0], 8 * sizeof (uint8_t));

    setIdBase();

    getIdBase(&readAddr[0]);

    TEST_ASSERT_TRUE(!memcmp(&fake1WAddress[0], &readAddr[0], 8));

    readAddr[3] = 0x3;

    TEST_ASSERT_FALSE(!memcmp(&fake1WAddress[0], &readAddr[0], 8));
}

void MCP7941X_GetAndSetTimeWorks(void) {
    struct tm temps;
    struct tm temps2;

    int64_t temps_epoch = 0;
    int64_t temps2_epoch = 0;

    temps.tm_mday = 28;
    temps.tm_mon = 7;
    temps.tm_year = 1 + 100; // 1 = 2001
    temps.tm_hour = 15;
    temps.tm_min = 25;
    temps.tm_sec = 0;

    MCP7941X_setTime(&temps);
    MCP7941X_getTime(&temps2);


    temps_epoch = mktime(&temps);
    temps2_epoch = mktime(&temps2);

    /*char buffer[26];
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S\n", &temps);
    puts(buffer);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S\n", &temps2);
    puts(buffer);*/

    TEST_ASSERT_EQUAL(temps_epoch, temps2_epoch);

    temps.tm_mday = 25;
    temps_epoch = mktime(&temps);

    TEST_ASSERT_FALSE(temps_epoch == temps2_epoch);
}

void RUN_MCP7941X_TESTS(void) {
    RUN_TEST(MCP7941X_ToBCDWorks);
    RUN_TEST(MCP7941X_fromBCDWorks);
    RUN_TEST(MCP7941X_setIdBaseWorks);
#ifdef TDD_SOFTWARE
    RUN_TEST(MCP7941X_getIdBaseWorks);
#endif
    RUN_TEST(MCP7941X_GetAndSetTimeWorks);
}

#endif