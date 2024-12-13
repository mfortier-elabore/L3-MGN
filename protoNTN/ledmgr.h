#ifndef ledmgr_h
#define ledmgr_h

#include "Arduino.h"

class LedManager {
public:
  LedManager(uint8_t pin);
  void update(void);
  void setEtat(uint8_t etat);
  void setFlashe(uint8_t flashe);
  void eteint(void);
  void allume(void);
  void toggle(void);

private:
  uint8_t etat;
  uint8_t flashe;
  uint8_t pin;
  uint64_t t_debut;

  const uint16_t TEMPS_FLASH = 500;
};

#endif  // ledmgr_h