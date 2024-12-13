extern "C"
{
#include "../../LIS2DTW12.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(LIS2DTW12)
{
    /*void setup()
    {
      LIS2DTW12_Create();
    }

    void teardown()
    {
       LIS2DTW12_Destroy();
    }*/
};

TEST(LIS2DTW12, RepondSurWhoAmII2C)
{
    uint8_t data = 0;
    fakeI2CDevices[LIS2DTW12_ADDR][WHO_AM_I] = 0x44;
    I2CHelper_ReadRegister(LIS2DTW12_ADDR, WHO_AM_I, &data);
    CHECK_EQUAL(0x44, data);
}

TEST(LIS2DTW12, SetupAjusteLesBonsRegistres)
{
    uint8_t data = 0;
    fakeI2CDevices[LIS2DTW12_ADDR][WHO_AM_I] = 0x44;
    I2CHelper_ReadRegister(LIS2DTW12_ADDR, WHO_AM_I, &data);
    CHECK_EQUAL(0x44, data);
}