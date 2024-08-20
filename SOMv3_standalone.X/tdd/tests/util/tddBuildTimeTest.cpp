#include "CppUTest/TestHarness.h"
#include "tddBuildTime.h"

TEST_GROUP(tddBuildTime)
{
  tddBuildTime* projectBuildTime;

  void setup()
  {
    projectBuildTime = new tddBuildTime();
  }
  void teardown()
  {
    delete projectBuildTime;
  }
};

TEST(tddBuildTime, Create)
{
  CHECK(0 != projectBuildTime->GetDateTime());
}

