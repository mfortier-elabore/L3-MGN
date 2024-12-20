/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 3.1.1
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

#include "../pins.h"


void PIN_MANAGER_Initialize(void)
{
   /**
    LATx registers
    */
    LATA = 0x0;
    LATB = 0x50;
    LATC = 0x0;
    /**
    ODx registers
    */
    ODCONA = 0x0;
    ODCONB = 0x50;
    ODCONC = 0x0;

    /**
    TRISx registers
    */
    TRISA = 0x3B;
    TRISB = 0x20;
    TRISC = 0xBA;

    /**
    ANSELx registers
    */
    ANSELA = 0x3;
    ANSELB = 0x0;
    ANSELC = 0x78;

    /**
    WPUx registers
    */
    WPUA = 0x30;
    WPUB = 0x20;
    WPUC = 0x0;


    /**
    SLRCONx registers
    */
    SLRCONA = 0x37;
    SLRCONB = 0xF0;
    SLRCONC = 0xFF;

    /**
    INLVLx registers
    */
    INLVLA = 0x3F;
    INLVLB = 0xF0;
    INLVLC = 0xFF;

   /**
    RxyI2C | RxyFEAT registers   
    */
    RB4I2C = 0x0;
    RB6I2C = 0x0;
    /**
    PPS registers
    */
    U1RXPPS = 0x17; //RC7->UART1:RX1;
    U2RXPPS = 0x11; //RC1->UART2:RX2;
    RB7PPS = 0x10;  //RB7->UART1:TX1;
    RC6PPS = 0x11;  //RC6->UART1:TXDE1;
    RC2PPS = 0x13;  //RC2->UART2:TX2;
    I2C1SCLPPS = 0xE;  //RB6->I2C1:SCL1;
    RB6PPS = 0x21;  //RB6->I2C1:SCL1;
    I2C1SDAPPS = 0xC;  //RB4->I2C1:SDA1;
    RB4PPS = 0x22;  //RB4->I2C1:SDA1;

   /**
    IOCx registers 
    */
    IOCAP = 0x0;
    IOCAN = 0x0;
    IOCAF = 0x0;
    IOCBP = 0x0;
    IOCBN = 0x0;
    IOCBF = 0x0;
    IOCCP = 0x0;
    IOCCN = 0x0;
    IOCCF = 0x0;


}
  
void PIN_MANAGER_IOC(void)
{
}
/**
 End of File
*/