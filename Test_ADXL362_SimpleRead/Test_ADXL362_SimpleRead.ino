/*
 ADXL362_SimpleRead.ino -  Simple XYZ axis reading example
 for Analog Devices ADXL362 - Micropower 3-axis accelerometer
 go to http://www.analog.com/ADXL362 for datasheet
 
 
 License: CC BY-SA 3.0: Creative Commons Share-alike 3.0. Feel free 
 to use and abuse this code however you'd like. If you find it useful
 please attribute, and SHARE-ALIKE!
 
 Created June 2012
 by Anne Mahaffey - hosted on http://annem.github.com/ADXL362

 Modified May 2013
 by Jonathan Ruiz de Garibay
 
Connect SCLK, MISO, MOSI, and CSB of ADXL362 to
SCLK, MISO, MOSI, and DP 10 of Arduino 
(check http://arduino.cc/en/Reference/SPI for details)
 
*/

#include <SPI.h>
#include <ADXL362.h>

#define SAMPLES_MOYENNE_LONG 1000
#define SAMPLES_MOYENNE_COURT 150

#define LED PC13


class DonneesAccel {
public:
  int16_t valeurs[SAMPLES_MOYENNE_LONG] = { 0 };
  float moyenneRapide, moyenneLente;
  double stdevRapide, stdevLente;
  int index = 0;
  int nb_court, nb_long;
  ADXL362* p_xl;

  DonneesAccel(ADXL362* p_xl, int nb_court, int nb_long) {
    this->p_xl = p_xl;
    this->nb_court = nb_court;
    this->nb_long = nb_long;

    this->lire();

    // Remplit la table avec la valeur initiale
    int16_t val_init = this->valeurs[0];
    for (int i = 0; i < nb_long; ++i) {
      this->valeurs[i] = val_init;
    }
  };

  void setValeurZ(int16_t val) {
    this->valeurs[index] = val;

    ++this->index;
    if (this->index >= nb_long) {
      this->index = 0;
    }
  }

  void lire() {
    int16_t XValue, YValue, ZValue, Temperature;
    this->p_xl->readXYZTData(XValue, YValue, ZValue, Temperature);
    this->setValeurZ(ZValue);

   /* Serial.print(ZValue);
    Serial.print(" a l'index ");    
    Serial.println(this->index);   */

    this->calcMoyenneLente();
    this->calcMoyenneRapide();
    this->calcStdevLente();
    this->calcStdevRapide();
    
  }

  int16_t getVal(int pos) {
    int16_t pos_corr = 0;

    if(this->index + pos > 0) {
      pos_corr = this->index + pos;
    } else {
      pos_corr = this->index + pos + nb_long - 1;
    }

    return this->valeurs[pos_corr];
  }

  void calcMoyenneLente() {
    float moyenne = 0;

    for (int i = 0; i < nb_long; ++i) {
      moyenne += this->valeurs[i];
    }

    moyenne = moyenne / nb_long;

    this->moyenneLente = moyenne;
  };

  void calcMoyenneRapide() {
    float moyenne = 0;

    for (int i = 0; i < nb_court; ++i) {
      moyenne += this->getVal(-i);
    }

    moyenne = moyenne / nb_court;

    this->moyenneRapide = moyenne;
  };

  void calcStdevLente() {
    double sq_sum = 0;

    for (int i = 0; i < this->nb_long; ++i) {
      sq_sum += this->valeurs[i] * this->valeurs[i];
    }

    double stdev = std::sqrt(sq_sum / this->nb_long - this->moyenneLente * this->moyenneLente);

    this->stdevLente = stdev;
  }

  void calcStdevRapide() {
    double sq_sum = 0;

    for (int i = 0; i < this->nb_court; ++i) {
      sq_sum += this->getVal(-i) * this->getVal(-i);
    }

    double stdev = std::sqrt(sq_sum / this->nb_court - this->moyenneRapide * this->moyenneRapide);

    this->stdevRapide = stdev;
  }

};

// Variables globales
ADXL362 xl;
DonneesAccel* donnees;

void setup() {

  pinMode(LED, OUTPUT);

  Serial.begin(9600);
  xl.begin(10);       // Setup SPI protocol, issue device soft reset
  xl.beginMeasure();  // Switch ADXL362 to measure mode

  Serial.println("Start Demo: Simple Read");

  donnees = new DonneesAccel(&xl, SAMPLES_MOYENNE_COURT, SAMPLES_MOYENNE_LONG);

  digitalWrite(LED, 1);
  delay(250);
  digitalWrite(LED, 0);
  delay(250);
  digitalWrite(LED, 1);
  delay(250);
  digitalWrite(LED, 0);
  delay(250);
  digitalWrite(LED, 1);
  delay(250);
  digitalWrite(LED, 0);
  delay(250);
}

void loop() {

  donnees->lire();

  Serial.print("Lente:");
  Serial.println(donnees->moyenneLente + 3 * donnees->stdevLente);

  Serial.print(",Rapide:");
  Serial.println(donnees->moyenneRapide + 3 * donnees->stdevRapide);

  delay(100);  // Arbitrary delay to make serial monitor easier to observe
}
