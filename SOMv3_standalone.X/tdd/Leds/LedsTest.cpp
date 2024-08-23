extern "C"
{
#include "../../leds.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(Leds)
{

    
    /*void setup()
    {
      Test_Create();
    }

    void teardown()
    {
       Test_Destroy();
    }*/
};

TEST(Leds, Create)
{
  CHECK_TRUE(1);
}

