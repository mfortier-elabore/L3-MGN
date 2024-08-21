extern "C"
{
#include "I2CHelper.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(I2CHelper)
{
    void setup()
    {
      I2CHelper_Create();
    }

    void teardown()
    {
       I2CHelper_Destroy();
    }
};

TEST(I2CHelper, Create)
{
  FAIL("Start here");
}

