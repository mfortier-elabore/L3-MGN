#ifndef RUNNING_HOURS_H
#define RUNNING_HOURS_H

#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>

#define ALPHA           0.2
#define WRITE_PERIOD    10

#define DIGITAL_IN_PIN A0

enum thresholdStatus_t {
    status_stopped,
    status_running
};

enum thresholdType_t {
    threshold_analog,
    threshold_digital
};

typedef struct Threshold_t {
    uint16_t movingAverage;
    uint16_t threshold;
    enum thresholdStatus_t status;
    enum thresholdType_t type;   // 0 : analog threshold, 1 : digital threshold
} Threshold_t;

typedef struct RunningHours_t {
    Threshold_t threshold;
    uint32_t * p_eepromAddress;
    uint8_t eepromWritePeriod;
    uint32_t runningSeconds;
    uint8_t count;
} RunningHours_t;

void Threshold_init(Threshold_t * threshold, uint16_t limitVal);
void Threshold_update(Threshold_t * threshold, uint16_t val);

void RunningHours_init(RunningHours_t * rh, uint32_t * eepromAddress, uint8_t writePeriod, uint16_t limitVal);
void RunningHours_update(RunningHours_t * rh, uint16_t value);

void RunningHours_readSavedValue(RunningHours_t * rh);
void RunningHours_saveValue(RunningHours_t * rh);

void MGN_RunningHours_init(void);
void MGN_RunningHours_update(void);

uint32_t MGN_RunningHours_getDigitalInCount(void);

#if defined(TDD_SOFTWARE) || defined(TDD_HARDWARE)
extern RunningHours_t lowAccelerometer, highAccelerometer, digitalIn;
#endif

#endif // RUNNING_HOURS_H