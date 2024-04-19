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


ADXL362 xl;

int16_t temp;
int16_t XValue, YValue, ZValue, Temperature;

uint16_t valeurs[SAMPLES_MOYENNE_LONG] = {0};
uint16_t valeurs_court[SAMPLES_MOYENNE_COURT] = {0};

void setup(){
  
  Serial.begin(9600);
  xl.begin(10);                   // Setup SPI protocol, issue device soft reset
  xl.beginMeasure();              // Switch ADXL362 to measure mode  
	
  Serial.println("Start Demo: Simple Read");

  xl.readXYZTData(XValue, YValue, ZValue, Temperature);

  for(int i = 0; i < SAMPLES_MOYENNE_LONG; ++i) {
    valeurs[i] = ZValue;
  }

  for(int i = 0; i < SAMPLES_MOYENNE_COURT; ++i) {
    valeurs_court[i] = ZValue;
  }


}

float calcMoyenne(){
  float moyenne = 0;

  for(int i = 0; i < SAMPLES_MOYENNE_LONG; ++i) {
    moyenne += valeurs[i];
  }

  moyenne = moyenne / SAMPLES_MOYENNE_LONG;

  return moyenne;
}

float calcMoyenneCourt(){
  float moyenne = 0;

  for(int i = 0; i < SAMPLES_MOYENNE_COURT; ++i) {
    moyenne += valeurs_court[i];
  }

  moyenne = moyenne / SAMPLES_MOYENNE_COURT;

  return moyenne;
}


double calcStdev(float moyenne) {
  double sq_sum = 0;

  for(int i = 0; i < SAMPLES_MOYENNE_LONG; ++i) {
    sq_sum += valeurs[i]*valeurs[i];
  }

  double stdev = std::sqrt(sq_sum / SAMPLES_MOYENNE_LONG - moyenne * moyenne);

  return stdev;
}

double calcStdevCourt(float moyenne) {
  double sq_sum = 0;

  for(int i = 0; i < SAMPLES_MOYENNE_COURT; ++i) {
    sq_sum += valeurs_court[i]*valeurs_court[i];
  }

  double stdev = std::sqrt(sq_sum / SAMPLES_MOYENNE_COURT - moyenne * moyenne);

  return stdev;
}

uint16_t i = 0;
uint16_t j = 0;

void loop(){
  
  // read all three axis in burst to ensure all measurements correspond to same sample time
  xl.readXYZTData(XValue, YValue, ZValue, Temperature);  
  Serial.print("ZVALUE:");
  Serial.println(ZValue);	

  valeurs[i] = ZValue;
  valeurs_court[j] = ZValue;

  ++i;
  if(i>=SAMPLES_MOYENNE_LONG) {
    i = 0;
  }

  ++j;
  if(j>=SAMPLES_MOYENNE_COURT) {
    j = 0;
  }

  float moyenne = calcMoyenne();
  double stdev = calcStdev(moyenne);

  float moyenneCourt = calcMoyenneCourt();
  double stdevCourt = calcStdevCourt(moyenneCourt);

  Serial.print(",MOY:");
  Serial.println(moyenne+3*stdev);

  Serial.print(",MOY2:");
  Serial.println(moyenneCourt+3*stdevCourt);

/*
  Serial.print(",lim+:");
  Serial.println(stdev*3+moyenne);
  
  Serial.print(",lim-:");
  Serial.println(moyenne-stdev*3);*/
   
  delay(100);                // Arbitrary delay to make serial monitor easier to observe

}

