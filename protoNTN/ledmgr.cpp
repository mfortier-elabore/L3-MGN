#include "ledmgr.h"

LedManager::LedManager(uint8_t pin) {
  this->etat = 0;
  this->flashe = 0;
  this->t_debut = 0;
  this->pin = pin;

  pinMode(this->pin, OUTPUT);
}

void LedManager::update() {
  /*Serial.print("Pin ");
  Serial.print(this->pin);
  Serial.print(" ");
  Serial.print(this->etat);
  Serial.print(" ");
  Serial.println(this->flashe);*/

  if (this->t_debut + TEMPS_FLASH < millis()) {
    this->t_debut = millis();
    if (this->etat) {
      if (this->flashe) {
        digitalWrite(this->pin, !digitalRead(this->pin));
      } else {
        digitalWrite(this->pin, HIGH);
      }
    } else {
      digitalWrite(this->pin, LOW);
    }
  }
}

void LedManager::setEtat(uint8_t etat) {
  this->etat = etat;
}

void LedManager::setFlashe(uint8_t flashe) {
  this->flashe = flashe;
}

void LedManager::eteint(void) {
  digitalWrite(this->pin, LOW);
}

void LedManager::allume(void) {
  digitalWrite(this->pin, HIGH);
}