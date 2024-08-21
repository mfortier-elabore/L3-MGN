/**
 * Generated Pins header File
 * 
 * @file pins.h
 * 
 * @defgroup  pinsdriver Pins Driver
 * 
 * @brief This is generated driver header for pins. 
 *        This header file provides APIs for all pins selected in the GUI.
 *
 * @version Driver Version  3.1.1
*/

/*
� [2024] Microchip Technology Inc. and its subsidiaries.

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

#ifndef PINS_H
#define PINS_H

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RA2 aliases
#define IO_1W_TRIS                 TRISAbits.TRISA2
#define IO_1W_LAT                  LATAbits.LATA2
#define IO_1W_PORT                 PORTAbits.RA2
#define IO_1W_WPU                  WPUAbits.WPUA2
#define IO_1W_OD                   ODCONAbits.ODCA2
#define IO_1W_ANS                  ANSELAbits.ANSELA2
#define IO_1W_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define IO_1W_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define IO_1W_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define IO_1W_GetValue()           PORTAbits.RA2
#define IO_1W_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define IO_1W_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define IO_1W_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define IO_1W_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define IO_1W_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define IO_1W_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define IO_1W_SetAnalogMode()      do { ANSELAbits.ANSELA2 = 1; } while(0)
#define IO_1W_SetDigitalMode()     do { ANSELAbits.ANSELA2 = 0; } while(0)

// get/set RB4 aliases
#define IO_SDA_TRIS                 TRISBbits.TRISB4
#define IO_SDA_LAT                  LATBbits.LATB4
#define IO_SDA_PORT                 PORTBbits.RB4
#define IO_SDA_WPU                  WPUBbits.WPUB4
#define IO_SDA_OD                   ODCONBbits.ODCB4
#define IO_SDA_ANS                  ANSELBbits.ANSELB4
#define IO_SDA_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define IO_SDA_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define IO_SDA_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define IO_SDA_GetValue()           PORTBbits.RB4
#define IO_SDA_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define IO_SDA_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define IO_SDA_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define IO_SDA_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define IO_SDA_SetPushPull()        do { ODCONBbits.ODCB4 = 0; } while(0)
#define IO_SDA_SetOpenDrain()       do { ODCONBbits.ODCB4 = 1; } while(0)
#define IO_SDA_SetAnalogMode()      do { ANSELBbits.ANSELB4 = 1; } while(0)
#define IO_SDA_SetDigitalMode()     do { ANSELBbits.ANSELB4 = 0; } while(0)

// get/set RB5 aliases
#define IO_RX_TRIS                 TRISBbits.TRISB5
#define IO_RX_LAT                  LATBbits.LATB5
#define IO_RX_PORT                 PORTBbits.RB5
#define IO_RX_WPU                  WPUBbits.WPUB5
#define IO_RX_OD                   ODCONBbits.ODCB5
#define IO_RX_ANS                  ANSELBbits.ANSELB5
#define IO_RX_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define IO_RX_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define IO_RX_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define IO_RX_GetValue()           PORTBbits.RB5
#define IO_RX_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define IO_RX_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define IO_RX_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define IO_RX_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define IO_RX_SetPushPull()        do { ODCONBbits.ODCB5 = 0; } while(0)
#define IO_RX_SetOpenDrain()       do { ODCONBbits.ODCB5 = 1; } while(0)
#define IO_RX_SetAnalogMode()      do { ANSELBbits.ANSELB5 = 1; } while(0)
#define IO_RX_SetDigitalMode()     do { ANSELBbits.ANSELB5 = 0; } while(0)

// get/set RB6 aliases
#define IO_SCL_TRIS                 TRISBbits.TRISB6
#define IO_SCL_LAT                  LATBbits.LATB6
#define IO_SCL_PORT                 PORTBbits.RB6
#define IO_SCL_WPU                  WPUBbits.WPUB6
#define IO_SCL_OD                   ODCONBbits.ODCB6
#define IO_SCL_ANS                  ANSELBbits.ANSELB6
#define IO_SCL_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define IO_SCL_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define IO_SCL_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define IO_SCL_GetValue()           PORTBbits.RB6
#define IO_SCL_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define IO_SCL_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define IO_SCL_SetPullup()          do { WPUBbits.WPUB6 = 1; } while(0)
#define IO_SCL_ResetPullup()        do { WPUBbits.WPUB6 = 0; } while(0)
#define IO_SCL_SetPushPull()        do { ODCONBbits.ODCB6 = 0; } while(0)
#define IO_SCL_SetOpenDrain()       do { ODCONBbits.ODCB6 = 1; } while(0)
#define IO_SCL_SetAnalogMode()      do { ANSELBbits.ANSELB6 = 1; } while(0)
#define IO_SCL_SetDigitalMode()     do { ANSELBbits.ANSELB6 = 0; } while(0)

// get/set RB7 aliases
#define IO_TX_TRIS                 TRISBbits.TRISB7
#define IO_TX_LAT                  LATBbits.LATB7
#define IO_TX_PORT                 PORTBbits.RB7
#define IO_TX_WPU                  WPUBbits.WPUB7
#define IO_TX_OD                   ODCONBbits.ODCB7
#define IO_TX_ANS                  ANSELBbits.ANSELB7
#define IO_TX_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define IO_TX_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define IO_TX_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define IO_TX_GetValue()           PORTBbits.RB7
#define IO_TX_SetDigitalInput()    do { TRISBbits.TRISB7 = 1; } while(0)
#define IO_TX_SetDigitalOutput()   do { TRISBbits.TRISB7 = 0; } while(0)
#define IO_TX_SetPullup()          do { WPUBbits.WPUB7 = 1; } while(0)
#define IO_TX_ResetPullup()        do { WPUBbits.WPUB7 = 0; } while(0)
#define IO_TX_SetPushPull()        do { ODCONBbits.ODCB7 = 0; } while(0)
#define IO_TX_SetOpenDrain()       do { ODCONBbits.ODCB7 = 1; } while(0)
#define IO_TX_SetAnalogMode()      do { ANSELBbits.ANSELB7 = 1; } while(0)
#define IO_TX_SetDigitalMode()     do { ANSELBbits.ANSELB7 = 0; } while(0)

// get/set RC0 aliases
#define IO_LED_TRIS                 TRISCbits.TRISC0
#define IO_LED_LAT                  LATCbits.LATC0
#define IO_LED_PORT                 PORTCbits.RC0
#define IO_LED_WPU                  WPUCbits.WPUC0
#define IO_LED_OD                   ODCONCbits.ODCC0
#define IO_LED_ANS                  ANSELCbits.ANSELC0
#define IO_LED_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define IO_LED_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define IO_LED_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define IO_LED_GetValue()           PORTCbits.RC0
#define IO_LED_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define IO_LED_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define IO_LED_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define IO_LED_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define IO_LED_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define IO_LED_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define IO_LED_SetAnalogMode()      do { ANSELCbits.ANSELC0 = 1; } while(0)
#define IO_LED_SetDigitalMode()     do { ANSELCbits.ANSELC0 = 0; } while(0)

/**
 * @ingroup  pinsdriver
 * @brief GPIO and peripheral I/O initialization
 * @param none
 * @return none
 */
void PIN_MANAGER_Initialize (void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt on Change Handling routine
 * @param none
 * @return none
 */
void PIN_MANAGER_IOC(void);


#endif // PINS_H
/**
 End of File
*/