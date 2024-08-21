/*
 * File:   leds.c
 * Author: mfortier
 *
 * Created on June 6, 2023, 11:06 AM
 *    Rev1 : 2024-08-20 - Modifications pour L3-MGN
 */

#include "leds.h"

#ifdef XC8_TOOLCHAIN
#else
uint8_t fakeLedPinValue = 0;
#endif

// deux variables globales pour la gestin des modes pour les dels
enum etatSortie_t etat_LED;
uint8_t nbTick50ms = 0; // Nombre de 50ms

/**
 * \brief Setter pour l'état de la led
 * @param etat Nouvel état de la led
 */
void setEtatLED(enum etatSortie_t etat) {
    if (etat_LED != etat_erreur && etat_LED != etat_configuration) {
        if (etat_LED != etat) {
            nbTick50ms = 0;
            etat_LED = etat;
        }
    }
}

/**
 * \brief Gestionnaire de l'état de la led
 */
void gestionLED(void) {
    if (etat_LED == etat_configuration) {
        if (nbTick50ms == LED_CONFIGURATION_PERIODE) {
            IO_LED_Toggle();
            nbTick50ms = 0;
        } else {
            nbTick50ms++;
        }
    } else if (etat_LED == etat_configDone) {
        IO_LED_SetHigh();
        nbTick50ms = 0;
    } else if (etat_LED == etat_operation) {
        if (nbTick50ms != 0) {
            IO_LED_SetLow();

            if (nbTick50ms >= LED_OPERATION_PERIODE) {
                nbTick50ms = 0;
                IO_LED_SetHigh();
            }
        } else {
            IO_LED_SetHigh();
        }
        nbTick50ms++;
    } else if (etat_LED == etat_standby) {
        if (nbTick50ms == 5) {
            IO_LED_Toggle();
            nbTick50ms = 0;
        } else {
            nbTick50ms++;
        }
    } else if (etat_LED == etat_erreur) { // 2s ON, 3s OFF
        if (nbTick50ms <= 40) {
            IO_LED_SetHigh();
            nbTick50ms++;
        } else if (nbTick50ms >= 41 && nbTick50ms < 100) {
            IO_LED_SetLow();
            nbTick50ms++;
        } else {
            nbTick50ms = 0;
        }
    } else {
        IO_LED_SetLow();
    }
}

/*
void tests_LED() {
    setEtatLED(etat_configuration);
    __delay_ms(10000);
    setEtatLED(etat_operation);
    __delay_ms(10000);
    setEtatLED(etat_standby);
    __delay_ms(10000);
    setEtatLED(etat_erreur);
    __delay_ms(10000);
}
 */
