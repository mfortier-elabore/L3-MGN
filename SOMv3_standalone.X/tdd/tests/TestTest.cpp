extern "C"
{
#include "Test.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(Test)
{
    void setup()
    {
      Test_Create();
    }

    void teardown()
    {
       Test_Destroy();
    }
};

TEST(Test, Create)
{
  FAIL("Start here");
}

