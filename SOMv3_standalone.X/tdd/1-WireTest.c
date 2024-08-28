#ifdef TDD_SIM

#include "AllTests.h"
#include "../1-wire.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void OneWire_owDriveSets1WPinTo0(void) {
    ow_release();
    ow_drive();
    TEST_ASSERT_EQUAL(0, IO_1W_GetValue());
}

void OneWire_owDriveSets1WPinToOutput(void) {
    ow_release();
    ow_drive();
    TEST_ASSERT_EQUAL(0, IO_1W_TRIS);
}

void OneWire_owReleaseSets1WPinTo1(void) {
    ow_drive();
    ow_release();
    TEST_ASSERT_EQUAL(1, IO_1W_LAT); // GetValue retourne faux
}

void OneWire_owReleaseSets1WPinToInput(void) {
    ow_drive();
    ow_release();
    TEST_ASSERT_EQUAL(1, IO_1W_TRIS);
}

void OneWire_doCRCFailsIfWrong(void) {
    uint8_t fake1WAddress_bad[8] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
    TEST_ASSERT_FALSE(do_crc(&fake1WAddress_bad[0], 8) == 0);
}

void OneWire_doCRCSucceedsIfGood() {
    uint8_t fake1WAddress_good[8] = {0x23, 0x16, 0xD9, 0x1A, 0x03, 0x00, 0x00, 0x2B};
    TEST_ASSERT_TRUE(do_crc(&fake1WAddress_good[0], 8) == 0);
}

void RUN_1WIRE_TESTS(void) {
    RUN_TEST(OneWire_owDriveSets1WPinTo0);
    RUN_TEST(OneWire_owDriveSets1WPinToOutput);
    RUN_TEST(OneWire_owReleaseSets1WPinTo1);
    RUN_TEST(OneWire_owReleaseSets1WPinToInput);
    RUN_TEST(OneWire_doCRCFailsIfWrong);
    RUN_TEST(OneWire_doCRCSucceedsIfGood);
}

#endif