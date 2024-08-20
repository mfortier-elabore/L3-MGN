/* 
 * File: leds.h
 * Author: MFortier
 * Comments:
 * Revision history:
 *      Initial : 2023-06-07
 */

#ifndef LEDS_H
#define	LEDS_H

#ifdef XC8_TOOLCHAIN
#include <xc.h>
#endif

// PWM de la LED
#define LED_CONFIGURATION_PERIODE                   1     // 50ms on / 50ms off
#define LED_OPERATION_PERIODE                       60    // 3000 ms off / 50ms on
#define LED_STANDBY_PERIODE                         5     // 250 ms on / 250ms off

// États possibles pour la sortie LED
enum etatSortie_t {
    etat_standby,
    etat_operation,
    etat_configuration,
    etat_configDone,
    etat_erreur
};

// Gestion led
void setEtatLED(enum etatSortie_t etat);
void gestionLED(void);
void tests_LED(void);

#endif	/* LEDS_H */