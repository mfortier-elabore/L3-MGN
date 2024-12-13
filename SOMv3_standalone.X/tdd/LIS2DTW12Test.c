#if defined(TDD_SOFTWARE) || defined(TDD_HARDWARE)

#include "AllTests.h"
#include "../LIS2DTW12.h"

/*void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}*/

/*void test_function_should_doBlahAndBlah(void) {
    //test stuff
}

void test_function_should_doAlsoDoBlah(void) {
    //more test stuff
}*/

void ACCEL_RespondsToI2C(void) {
    uint8_t data;
    int registerAddress = LIS2DTW12_WHO_AM_I;
    
    I2CHelper_ReadRegister(LIS2DTW12_ADDR, &registerAddress, &data);
    
    TEST_ASSERT_EQUAL(0x44, data);
}

void ACCEL_GetsConfiguredProperly(void) {
    uint8_t data;
    uint8_t data_ctrl1 = 0x14;
    uint8_t data_ctrl6 = 0x04;
    uint8_t data_fifo = 0x00;
    
    
    LIS2DTW12_Create();

    I2CHelper_ReadRegister(LIS2DTW12_ADDR, &LIS2DTW12_CTRL1, &data);
    TEST_ASSERT_EQUAL(data_ctrl1, data);
    
    I2CHelper_ReadRegister(LIS2DTW12_ADDR, &LIS2DTW12_CTRL6, &data);
    TEST_ASSERT_EQUAL(data_ctrl6, data);
    
    I2CHelper_ReadRegister(LIS2DTW12_ADDR, &LIS2DTW12_FIFO_CTRL, &data);
    TEST_ASSERT_EQUAL(data_fifo, data);
    
    TEST_ASSERT_EQUAL(0, accel.fastMovingAverage);
    TEST_ASSERT_EQUAL(0, accel.slowMovingAverage);
    TEST_ASSERT_EQUAL(0, accel.filteredValue);
    TEST_ASSERT_EQUAL(0, accel.lastValue);
}

void ACCEL_ReadsZAxisProperly(void) {
    int16_t data = 0x00;
    uint8_t tolerance = 500;
    uint16_t expectedVal = 16000;

    data = LIS2DTW12_ReadZAxis();
    
    uint8_t result = data >= (expectedVal - tolerance) && data <= (expectedVal + tolerance);
    
    TEST_ASSERT_TRUE(result);
}

void RUN_ACCELEROMETER_TESTS(void) {
    RUN_TEST(ACCEL_RespondsToI2C);
    RUN_TEST(ACCEL_GetsConfiguredProperly);
    RUN_TEST(ACCEL_ReadsZAxisProperly);
}

#endif
