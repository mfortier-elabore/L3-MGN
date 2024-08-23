#include "../unity/unity_fixture.h"
#include "../../1-wire.h"

TEST_GROUP(OneWire);

TEST_SETUP(OneWire)
{
}

TEST_TEAR_DOWN(OneWire)
{
}

TEST(OneWire, owDriveReleaseWorks)
{
    ow_release();
    CHECK_EQUAL(fake1wPinDirection, 1);
    CHECK_EQUAL(fake1wPinValue, 1);

    ow_drive();
    CHECK_EQUAL(fake1wPinDirection, 0);
    CHECK_EQUAL(fake1wPinValue, 0);
}


TEST(OneWire, doCRCFailsIfWrong) {
    uint8_t fake1WAddress_bad[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
    CHECK_FALSE(do_crc(&fake1WAddress_bad[0], 8)==0);
}

TEST(OneWire, doCRCSucceedsIfGood) {
    uint8_t fake1WAddress_good[8] = { 0x23, 0x16, 0xD9, 0x1A, 0x03, 0x00, 0x00, 0x2B };
    CHECK_TRUE(do_crc(&fake1WAddress_good[0], 8)==0);
}
