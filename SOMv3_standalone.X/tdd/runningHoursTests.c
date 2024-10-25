#if defined(TDD_SOFTWARE) || defined(TDD_HARDWARE)

#include "AllTests.h"
#include "../runningHours.h"

void RUNNINGHOURS_ThresholdGetsInitiatedProperly(void) {
    Threshold_t thresh;
    uint16_t limitVal = 22;

    Threshold_init(&thresh, limitVal);

    TEST_ASSERT_EQUAL(limitVal, thresh.threshold);
    TEST_ASSERT_EQUAL(status_stopped, thresh.status);
    TEST_ASSERT_EQUAL(0, thresh.movingAverage);
}

void RUNNINGHOURS_ThresholdCalculatesMovingAverageCorrectly(void) {
    Threshold_t thresh;
    uint16_t limitVal = 22;
    Threshold_init(&thresh, limitVal);
    uint16_t val = 0;

    for (uint8_t i = 0; i <= 100; ++i) {
        Threshold_update(&thresh, val++);
    }

    // 96 : résultat calculé avec ALPHA 0.2
    TEST_ASSERT_EQUAL(96, thresh.movingAverage);
}

void RUNNINGHOURS_ThresholdSwitchesWhenConditionMet(void) {
    Threshold_t thresh;
    uint16_t limitVal = 22;
    Threshold_init(&thresh, limitVal);

    for (uint8_t i = 0; i < 6; ++i) {
        Threshold_update(&thresh, 30);
    }

    // Avec alpha 0.2, 6 valeurs de 30 = 21 (trop bas)
    TEST_ASSERT_EQUAL(status_stopped, thresh.status);


    // Avec ALPHA 0.2, besoin de 7 valeurs de 30 pour atteindre 22
    Threshold_update(&thresh, 30);
    TEST_ASSERT_EQUAL(status_running, thresh.status);


    // Avec ALPHA 0.2, besoin d'une seule valeur a 20 pour redescendre en base de 22
    Threshold_update(&thresh, 20);
    TEST_ASSERT_EQUAL(status_stopped, thresh.status);
}

void RUNNINGHOURS_GetsInitiatedProperly(void) {
    RunningHours_t rh, rh_bidon;
    uint32_t eepromAddress = 0x380028U;
    uint8_t writePeriod = 10;

    // Pour écrire running hours a une adresse spécifique en eeprom
    RunningHours_init(&rh_bidon, &eepromAddress, writePeriod, 0);
    rh_bidon.runningSeconds = 12822L * 60L;
    RunningHours_saveValue(&rh_bidon);

    RunningHours_init(&rh, &eepromAddress, writePeriod, 10);

    TEST_ASSERT_EQUAL(eepromAddress, *(rh.p_eepromAddress));
    TEST_ASSERT_EQUAL(writePeriod, rh.eepromWritePeriod);
    TEST_ASSERT_EQUAL(10, rh.threshold.threshold);
    TEST_ASSERT_EQUAL(12822L * 60L, rh.runningSeconds);
}

void RUNNINGHOURS_DiscardsEEPROMReadingIfNotInitialized(void) {
    RunningHours_t rh, rh_bidon;
    uint32_t eepromAddress = 0x380028U;
    uint8_t writePeriod = 10;
    enum thresholdStatus_t threshold = status_stopped;

    // Pour initialiser eeprom a 0xFF (comme si pas ecrit)
    RunningHours_init(&rh_bidon, &eepromAddress, writePeriod, 0);
    rh_bidon.runningSeconds = 0xFFFFFFFFL;
    RunningHours_saveValue(&rh_bidon);

    RunningHours_init(&rh, &eepromAddress, writePeriod, 0);

    TEST_ASSERT_EQUAL(0x0L, rh.runningSeconds);
}

void RUNNINGHOURS_CountsIfEnabled(void) {
    RunningHours_t rh;
    uint32_t eepromAddress = 0x380028U;
    uint8_t writePeriod = 10;
    uint8_t threshold = 1;
    uint32_t startVal, endVal;

    RunningHours_init(&rh, &eepromAddress, writePeriod, threshold);

    // Met a running
    Threshold_update(&rh.threshold, 10);

    startVal = rh.runningSeconds;

    for (uint8_t i = 0; i < 10; ++i) {
        RunningHours_update(&rh, 10);
    }

    endVal = rh.runningSeconds;

    TEST_ASSERT_EQUAL(10 * 2, endVal - startVal);
}

void RUNNINGHOURS_DoesNotCountIfDisabled(void) {
    RunningHours_t rh;
    uint32_t eepromAddress = 0x380028U;
    uint8_t writePeriod = 10;
    uint8_t threshold = 10;
    uint32_t startVal, endVal;

    RunningHours_init(&rh, &eepromAddress, writePeriod, threshold);

    startVal = rh.runningSeconds;

    for (uint8_t i = 0; i < 10; ++i) {
        RunningHours_update(&rh, 10);
    }

    endVal = rh.runningSeconds;

    TEST_ASSERT_EQUAL(0, endVal - startVal);
}

void RUNNINGHOURS_SavesWhenPeriodHasElapsed(void) {
    RunningHours_t rh;
    uint32_t eepromAddress = 0x380028U;
    uint8_t writePeriod = 20;
    uint8_t threshold = 0;
    uint32_t startVal, endVal;

    RunningHours_init(&rh, &eepromAddress, writePeriod, threshold);
    Threshold_update(&rh.threshold, 10); // running

    startVal = rh.runningSeconds;

    for (uint8_t i = 0; i < 9; ++i) {
        RunningHours_update(&rh, 10);
    }

    RunningHours_init(&rh, &eepromAddress, writePeriod, threshold);
    Threshold_update(&rh.threshold, 10);

    endVal = rh.runningSeconds;

    TEST_ASSERT_EQUAL(0, endVal - startVal);

    for (uint8_t i = 0; i < 10; ++i) {
        RunningHours_update(&rh, 10);
    }

    RunningHours_init(&rh, &eepromAddress, writePeriod, threshold);

    endVal = rh.runningSeconds;

    TEST_ASSERT_EQUAL(20, endVal - startVal);
}

void RUNNINGHOURS_MGNInitWorksProperly(void) {
    MGN_RunningHours_init();
    
    TEST_ASSERT_EQUAL(0, MGN_RunningHours_getLowAccelCount());
    TEST_ASSERT_EQUAL(0, MGN_RunningHours_getHighAccelCount());
    TEST_ASSERT_EQUAL(0, MGN_RunningHours_getDigitalInCount());
}

void RUNNINGHOURS_MGNCountsCorrectly(void) {
    MGN_RunningHours_init();

    for (uint8_t i = 0; i < 10; ++i) {
        RunningHours_update(&lowAccelerometer, 0);
        RunningHours_update(&highAccelerometer, 0);
        RunningHours_update(&digitalIn, 0);
    }
    
    TEST_ASSERT_EQUAL(0, MGN_RunningHours_getLowAccelCount());
    TEST_ASSERT_EQUAL(0, MGN_RunningHours_getHighAccelCount());
    TEST_ASSERT_EQUAL(0, MGN_RunningHours_getDigitalInCount());
    
    for (uint8_t i = 0; i < 10; ++i) {
        RunningHours_update(&lowAccelerometer, 20);
        RunningHours_update(&highAccelerometer, 30);
        RunningHours_update(&digitalIn, 1);
    }
    
    // Avec ALPHA=0.2, résultats attendus -> 14, 10 20
    // Digital in : pas de moyenne mobile
    TEST_ASSERT_EQUAL(14, MGN_RunningHours_getLowAccelCount());
    TEST_ASSERT_EQUAL(10, MGN_RunningHours_getHighAccelCount());
    TEST_ASSERT_EQUAL(20, MGN_RunningHours_getDigitalInCount());
}

void RUN_RUNNING_HOURS_TESTS(void) {
    RUN_TEST(RUNNINGHOURS_ThresholdGetsInitiatedProperly);
    RUN_TEST(RUNNINGHOURS_ThresholdCalculatesMovingAverageCorrectly);
    RUN_TEST(RUNNINGHOURS_ThresholdSwitchesWhenConditionMet);
    RUN_TEST(RUNNINGHOURS_GetsInitiatedProperly);
    RUN_TEST(RUNNINGHOURS_DiscardsEEPROMReadingIfNotInitialized);
    RUN_TEST(RUNNINGHOURS_CountsIfEnabled);
    RUN_TEST(RUNNINGHOURS_DoesNotCountIfDisabled);
    RUN_TEST(RUNNINGHOURS_SavesWhenPeriodHasElapsed);
    RUN_TEST(RUNNINGHOURS_MGNInitWorksProperly);
    RUN_TEST(RUNNINGHOURS_MGNCountsCorrectly);
}

#endif