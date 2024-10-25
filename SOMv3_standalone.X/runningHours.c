#include "runningHours.h"
#include "LIS2DTW12.h"
#include "CFH.h"
#include "LIS2DTW12.h"

void Threshold_updateMovingAverage(Threshold_t * t, uint16_t val);

RunningHours_t lowAccelerometer, highAccelerometer, digitalIn;

void Threshold_init(Threshold_t * t, uint16_t limitVal) {
    t->movingAverage = 0;
    t->status = status_stopped;
    t->threshold = limitVal;
    t->type = threshold_analog;
    
    // Type is digital of limitVal == 1
    if(t->threshold == 1) {
        t->type = threshold_digital;
    }
}

void Threshold_update(Threshold_t * t, uint16_t val) {
    if(t->type == threshold_analog) {
        Threshold_updateMovingAverage(t, val);
    } else { // threshold_digital
        t->movingAverage = val;
    }

    if (t->movingAverage >= t->threshold) {
        t->status = status_running;
    } else {
        t->status = status_stopped;
    }
}

void Threshold_updateMovingAverage(Threshold_t * t, uint16_t val) {
    t->movingAverage = (uint16_t) (ALPHA * val + (1 - ALPHA) * t->movingAverage);
}

void RunningHours_init(RunningHours_t * rh, uint32_t * eepromAddress, uint8_t writePeriod, uint16_t limitVal) {
    Threshold_init(&rh->threshold, limitVal);
    rh->p_eepromAddress = eepromAddress;
    rh->eepromWritePeriod = writePeriod;
    rh->runningSeconds = 0;
    rh->count = 0;
    RunningHours_readSavedValue(rh);
}

void RunningHours_update(RunningHours_t * rh, uint16_t value) {
    Threshold_update(&rh->threshold, value);
    
    if (rh->threshold.status == status_running) {
        rh->runningSeconds += 2;

        rh->count += 2;

        if (rh->count >= rh->eepromWritePeriod) {
            RunningHours_saveValue(rh);
            rh->count = 0;
        }
    }
}

void RunningHours_readSavedValue(RunningHours_t * rh) {
    uint32_t savedValue = 0;
    uint8_t byte;

    for (uint8_t i = 0; i < 4; ++i) {
        byte = EEPROM_Read(*(rh->p_eepromAddress) + i);
        savedValue |= (0xffffffff & byte) << (i * 8 * sizeof (uint8_t));
    }

    if (savedValue == 0xFFFFFFFFL) {
        savedValue = 0;
    }

    rh->runningSeconds = savedValue;
}

void RunningHours_saveValue(RunningHours_t * rh) {
    for (uint8_t i = 0; i < 4; ++i) {
        uint8_t byte = (rh->runningSeconds >> (i * 8 * sizeof (uint8_t))) & 0xff;
        EEPROM_Write(*(rh->p_eepromAddress) + i, byte);
    }
}

void MGN_RunningHours_init(void) {
    RunningHours_init(&lowAccelerometer, &MEMORY_ADDRESS_RUNNING_HOURS_LOWACCEL, WRITE_PERIOD, 30);
    RunningHours_init(&highAccelerometer, &MEMORY_ADDRESS_RUNNING_HOURS_HIGHACCEL, WRITE_PERIOD, 80);
    RunningHours_init(&digitalIn, &MEMORY_ADDRESS_RUNNING_HOURS_DIGITALIN, WRITE_PERIOD, 1);
}

void MGN_RunningHours_update(void) {
    LIS2DTW12_Update();
    
    RunningHours_update(&lowAccelerometer, LIS2DTW12_GetValue());
    RunningHours_update(&highAccelerometer, LIS2DTW12_GetValue());
    
    RunningHours_update(&digitalIn, !IO_RA4_GetValue());
}

uint32_t MGN_RunningHours_getLowAccelCount(void) {
    return lowAccelerometer.runningSeconds;
}

uint32_t MGN_RunningHours_getHighAccelCount(void) {
    return highAccelerometer.runningSeconds;
}

uint32_t MGN_RunningHours_getDigitalInCount(void) {
    return digitalIn.runningSeconds;
}