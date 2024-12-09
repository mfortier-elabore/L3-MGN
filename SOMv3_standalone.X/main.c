/* 
 * File: main.c
 * Author: MFortier
 * Comments:
 * Revision history:
 *      Initial : 2023-04-03
 *      Rev1 : 2024-08-20 - Modifications pour L3-MGN
 */
/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
 */
#ifndef TDD_SOFTWARE
#include "mcc_generated_files/system/system.h"
#include "CFH.h"
#else
#define CLRWDT()              do { } while(0);
#endif

#if defined(TDD_SOFTWARE) || defined(TDD_HARDWARE)
#include "tdd/AllTests.h"
#endif

#include "runningHours.h"

/*
    Main application
 */

int main(void) {
#ifndef TDD_SOFTWARE
    SYSTEM_Initialize();
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts 
    // Use the following macros to: 

    // Ajout pour pin CTS, mappee a une entree non implmentee = RD0
    U1CTSPPS = 0x18;

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Deverouille le EEPROM
    NVM_UnlockKeySet(UNLOCK_KEY);

    TMR0_OverflowCallbackRegister(TMR0_2s_ISR);
    TMR2_OverflowCallbackRegister(gestionLED);
#endif

    MGN_RunningHours_init();

#if defined(TDD_SOFTWARE) || defined(TDD_HARDWARE)

    RunAllTests();

    while (1) {
        CLRWDT();
        if (update1W) {
            update1W = false;
            MGN_RunningHours_update();
            printf("\nlow : %lu\thigh: %lu\tdigIn : %lu",
                    MGN_RunningHours_getLowAccelCount(),
                    MGN_RunningHours_getHighAccelCount(),
                    MGN_RunningHours_getDigitalInCount());
        }
    }
#else

    printf("\n================ Demarrage\n");

    while (1) {

        if (update1W) {
            update1W = false;
            pollingCartouche();
            MGN_RunningHours_update();
            printf("\nlow : %lu\thigh: %lu\tdigIn : %lu",
                    MGN_RunningHours_getLowAccelCount(),
                    MGN_RunningHours_getHighAccelCount(),
                    MGN_RunningHours_getDigitalInCount());
        }

        CLRWDT();
    }
#endif
}
