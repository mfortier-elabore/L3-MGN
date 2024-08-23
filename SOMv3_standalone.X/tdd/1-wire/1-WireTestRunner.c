#include "../unity/unity_fixture.h"

TEST_GROUP_RUNNER(OneWire)
{
	TEST(OneWire, owDriveReleaseWorks);
	TEST(OneWire, doCRCFailsIfWrong);
	TEST(OneWire, doCRCSucceedsIfGood);
}

