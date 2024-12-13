#include "runningHours.h"

void Threshold_updateMovingAverage(Threshold_t* t, uint16_t val);

RunningHours_t digitalIn;


const uint32_t MEMORY_ADDRESS_RUNNING_HOURS_DIGITALIN = 0;

void Threshold_init(Threshold_t* t, uint16_t limitVal) {
  t->movingAverage = 0;
  t->status = status_stopped;
  t->threshold = limitVal;
  t->type = threshold_analog;

  // Type is digital of limitVal == 1
  if (t->threshold == 1) {
    t->type = threshold_digital;
  }
}

void Threshold_update(Threshold_t* t, uint16_t val) {
  if (t->type == threshold_analog) {
    Threshold_updateMovingAverage(t, val);
  } else {  // threshold_digital
    t->movingAverage = val;
  }

  if (t->movingAverage >= t->threshold) {
    if (t->status != status_running) {
      Serial.println("Running.");
    }
    t->status = status_running;
  } else {
    if (t->status != status_stopped) {
      Serial.println("Stopped.");
    }
    t->status = status_stopped;
  }
}

void Threshold_updateMovingAverage(Threshold_t* t, uint16_t val) {
  t->movingAverage = (uint16_t)(ALPHA * val + (1 - ALPHA) * t->movingAverage);
}

void RunningHours_init(RunningHours_t* rh, uint32_t* eepromAddress, uint8_t writePeriod, uint16_t limitVal) {
  Threshold_init(&rh->threshold, limitVal);
  rh->p_eepromAddress = eepromAddress;
  rh->eepromWritePeriod = writePeriod;
  rh->runningSeconds = 0;
  rh->count = 0;
  RunningHours_readSavedValue(rh);
}

void RunningHours_update(RunningHours_t* rh, uint16_t value) {
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

void RunningHours_readSavedValue(RunningHours_t* rh) {
  uint32_t savedValue = 0;
  uint8_t byte;

  for (uint8_t i = 0; i < 4; ++i) {
    byte = EEPROM.read(*(rh->p_eepromAddress) + i);
    savedValue |= (0xffffffff & byte) << (i * 8 * sizeof(uint8_t));
  }

  if (savedValue == 0xFFFFFFFFL) {
    savedValue = 0;
  }

  rh->runningSeconds = savedValue;

  Serial.print("Secondes fonctionnement en memoire : ");
  Serial.println(rh->runningSeconds);
}

void RunningHours_saveValue(RunningHours_t* rh) {
  for (uint8_t i = 0; i < 4; ++i) {
    uint8_t byte = (rh->runningSeconds >> (i * 8 * sizeof(uint8_t))) & 0xff;
    EEPROM.write(*(rh->p_eepromAddress) + i, byte);
  }

  Serial.print("Nouvelle valeur ecrite : ");
  Serial.println(rh->runningSeconds);
}

void MGN_RunningHours_init(void) {
  RunningHours_init(&digitalIn, &MEMORY_ADDRESS_RUNNING_HOURS_DIGITALIN, WRITE_PERIOD, 1);
  pinMode(DIGITAL_IN_PIN, INPUT_PULLUP);
}

void MGN_RunningHours_update(void) {
  static int64_t dernier_update = 0;

  if (dernier_update + 2000 < millis()) {
    dernier_update = millis();
    RunningHours_update(&digitalIn, !digitalRead(DIGITAL_IN_PIN));
  }
}

uint32_t MGN_RunningHours_getDigitalInCount(void) {
  return digitalIn.runningSeconds;
}