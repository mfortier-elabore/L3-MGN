/*
 L3-MGN - Test avec ADXL362

 Le but est de détecter que l'équipement est en marche, en suivant 
 la variation des vibrations détectées.

 MFortier - 2024-04-22
 
*/

#include <SPI.h>
#include <ADXL362.h>

#define SAMPLES_MOYENNE_LONG 200
#define SAMPLES_MOYENNE_COURT 50
#define ETAT_ARRET 0
#define ETAT_MARCHE 1


#define LED PC13
#define LED1 PB7
#define LED2 PB8
#define LED3 PB9


class Running {
public:
  float seuil;
  int compte, limite, led = 0;
  uint8_t etat;  // 0 = arrete, 1 = en marche

  Running(float seuil, int limite, int led) {
    this->seuil = seuil;
    this->limite = limite;
    this->etat = 0;
    this->led = led;
  }

  void demarrage() {
    this->etat = ETAT_MARCHE;
    digitalWrite(this->led, HIGH);
  }

  void arret() {
    this->etat = ETAT_ARRET;
    digitalWrite(this->led, LOW);
  }

  uint8_t update(double var) {
    if (this->etat == ETAT_ARRET) {
      if (var > this->seuil) {
        ++compte;
      } else {
        this->compte = 0;
      }
      if (this->compte > this->limite) {
        this->demarrage();
      }
    } else {
      if (var <= this->seuil) {
        --compte;
      } else {
        this->compte = 0;
      }
      if (this->compte < -this->limite) {
        this->arret();
      }
    }
    return this->etat;
  }
};

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

    for (int i = 0; i < this->nb_long; ++i) {
      this->lire();
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

    if (this->index + pos > 0) {
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

Running* niveau1;
Running* niveau2;
Running* niveau3;

void setup() {

  pinMode(LED, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  Serial.begin(9600);
  xl.begin(10);       // Setup SPI protocol, issue device soft reset
  xl.beginMeasure();  // Switch ADXL362 to measure mode

  Serial.println("Start Demo: Simple Read");

  donnees = new DonneesAccel(&xl, SAMPLES_MOYENNE_COURT, SAMPLES_MOYENNE_LONG);
  niveau1 = new Running(6, 60, LED2);
  niveau2 = new Running(12, 60, LED3);
  niveau3 = new Running(20, 60, LED1); 

  digitalWrite(LED, 0);
  delay(250);
  digitalWrite(LED, 1);/*
  digitalWrite(LED1, 1);
  delay(250);
  digitalWrite(LED1, 0);
  digitalWrite(LED2, 1);
  delay(250);
  digitalWrite(LED2, 0);
  digitalWrite(LED3, 1);
  delay(250);
  digitalWrite(LED3, 0);*/
}

void loop() {

  donnees->lire();

  double diff = (donnees->moyenneLente + 3 * donnees->stdevLente) - (donnees->moyenneRapide + 3 * donnees->stdevRapide);

  int depart = millis();
  Serial.print("Diff:");
  Serial.println(diff);

  niveau1->update(donnees->stdevRapide);
  niveau2->update(donnees->stdevRapide);
  niveau3->update(donnees->stdevRapide);
  
  Serial.print(",Niv1:");
  Serial.println(niveau1->etat);
  Serial.print(",Niv2:");
  Serial.println(niveau2->etat);
  Serial.print(",Niv3:");
  Serial.println(niveau3->etat);

  Serial.print(",stdevLente:");
  Serial.println(donnees->stdevLente);
  Serial.print(",stdevRapide:");
  Serial.println(donnees->stdevRapide);

  Serial.print(",Max:");
  Serial.println(2);
  Serial.print(",Max2:");
  Serial.println(6);
  Serial.print(",Min:");
  Serial.println(-2);

  if(niveau3->etat) {
    digitalWrite(niveau3->led, HIGH);
    digitalWrite(niveau2->led, LOW);
    digitalWrite(niveau1->led, LOW);
  } else if(niveau2->etat) {
    digitalWrite(niveau3->led, LOW);
    digitalWrite(niveau2->led, HIGH);
    digitalWrite(niveau1->led, LOW);    
  } else if(niveau1->etat) {
    digitalWrite(niveau3->led, LOW);
    digitalWrite(niveau2->led, LOW);
    digitalWrite(niveau1->led, HIGH);    
  } else {
    digitalWrite(niveau3->led, LOW);
    digitalWrite(niveau2->led, LOW);
    digitalWrite(niveau1->led, LOW);    
  }

  delay(100);
}
