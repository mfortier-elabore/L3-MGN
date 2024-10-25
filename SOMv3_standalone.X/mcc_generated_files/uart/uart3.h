/**
 * UART3 Generated Driver API Header File
 * 
 * @file uart3.h
 * 
 * @defgroup uart3 UART3
 * 
 * @brief This file contains API prototypes and other data types for the the Universal Asynchronous Receiver and Transmitter (UART) module.
 *
 * @version UART3 Driver Version 3.0.6
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

#ifndef UART3_H
#define UART3_H


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "../system/system.h"
#include "uart_drv_interface.h"

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

#define UART3_interface UART3


#define UART3_Initialize     UART3_Initialize
#define UART3_Deinitialize   UART3_Deinitialize
#define UART3_Write          UART3_Write
#define UART3_Read           UART3_Read
#define UART3__IsRxReady     UART3_IsRxReady
#define UART3_IsTxReady      UART3_IsTxReady
#define UART3_IsTxDone       UART3_IsTxDone

#define UART3_TransmitEnable       UART3_TransmitEnable
#define UART3_TransmitDisable      UART3_TransmitDisable
#define UART3_AutoBaudSet          UART3_AutoBaudSet
#define UART3_AutoBaudQuery        UART3_AutoBaudQuery
#define UART3_AutoBaudDetectCompleteReset  UART3_AutoBaudDetectCompleteReset
#define UART3_IsAutoBaudDetectOverflow     UART3_IsAutoBaudDetectOverflow
#define UART3_AutoBaudDetectOverflowReset  UART3_AutoBaudDetectOverflowReset
#define UART3_BRGCountSet               (NULL)
#define UART3_BRGCountGet               (NULL)
#define UART3_BaudRateSet               (NULL)
#define UART3_BaudRateGet               (NULL)
#define UART3__AutoBaudEventEnableGet   (NULL)
#define UART3_ErrorGet             UART3_ErrorGet

#define UART3_TxCompleteCallbackRegister     (NULL)
#define UART3_RxCompleteCallbackRegister      (NULL)
#define UART3_TxCollisionCallbackRegister  (NULL)
#define UART3_FramingErrorCallbackRegister UART3_FramingErrorCallbackRegister
#define UART3_OverrunErrorCallbackRegister UART3_OverrunErrorCallbackRegister
#define UART3_ParityErrorCallbackRegister  UART3_ParityErrorCallbackRegister
#define UART3_EventCallbackRegister        (NULL)

/**
 @ingroup uart3
 @struct uart3_status_t
 @brief This is an instance of UART3_STATUS for UART3 module
 */
typedef union {
    struct {
        uint8_t perr : 1;     /**<This is a bit field for Parity Error status*/
        uint8_t ferr : 1;     /**<This is a bit field for Framing Error status*/
        uint8_t oerr : 1;     /**<This is a bit field for Overfrun Error status*/
        uint8_t reserved : 5; /**<Reserved*/
    };
    size_t status;            /**<Group byte for status errors*/
}uart3_status_t;


/**
 * @ingroup uart3
 * @brief External object for uart_drv_interface.
 */
extern const uart_drv_interface_t UART3;

/**
 * @ingroup uart3
 * @brief Initializes the UART3 module. This routine is called
 *        only once during system initialization, before calling other APIs.
 * @param None.
 * @return None.
 */
void UART3_Initialize(void);

/**
 * @ingroup uart3
 * @brief Deinitializes and disables the UART3 module.

 * @param None.
 * @return None.
 */
void UART3_Deinitialize(void);

/**
 * @ingroup uart3
 * @brief Enables the UART3 module.     
 * @param None.
 * @return None.
 */
inline void UART3_Enable(void);

/**
 * @ingroup uart3
 * @brief Disables the UART3 module.
 * @param None.
 * @return None.
 */
inline void UART3_Disable(void);

/**
 * @ingroup uart3
 * @brief Enables the UART3 transmitter. This routine also enables
 *        UART3 to send bytes over the TX pin.
 * @param None.
 * @return None.
 */
inline void UART3_TransmitEnable(void);

/**
 * @ingroup uart3
 * @brief Disables the UART3 transmitter.
 * @param None.
 * @return None.
 */
inline void UART3_TransmitDisable(void);

/**
 * @ingroup uart3
 * @brief Enables the UART3 receiver. This routine also enables
 *        UART3 to send bytes over the RX pin.
 * @param None.
 * @return None.
 */
inline void UART3_ReceiveEnable(void);

/**
 * @ingroup uart3
 * @brief Disables the UART3 receiver.
 * @param None.
 * @return None.
 */
inline void UART3_ReceiveDisable(void);

/**
 * @ingroup uart3
 * @brief Enables the UART3 Send Break Control bit.
 * @param None.
 * @return None.
 */
inline void UART3_SendBreakControlEnable(void);

/**
 * @ingroup uart3
 * @brief Disables the UART3 Send Break Control bit.
 * @param None.
 * @return None.
 */
inline void UART3_SendBreakControlDisable(void);

/**
 * @ingroup uart3
 * @brief Enables the UART3 Auto-Baud Detection bit.
 * @param bool enable
 * @return None.
 */
inline void UART3_AutoBaudSet(bool enable);


/**
 * @ingroup uart3
 * @brief Reads the UART3 Auto-Baud Detection Complete bit.
 * @param None.
 * @return None.
 */
inline bool UART3_AutoBaudQuery(void);

/**
 * @ingroup uart3
 * @brief Resets the UART3 Auto-Baud Detection Complete bit.
 * @param None.
 * @return None.
 */
inline void UART3_AutoBaudDetectCompleteReset(void);

/**
 * @ingroup uart3
 * @brief Reads the UART3 Auto-Baud Detection Overflow bit.
 * @param None.
 * @return None.
 */
inline bool UART3_IsAutoBaudDetectOverflow(void);

/**
 * @ingroup uart3
 * @brief Resets the UART3 Auto-Baud Detection Overflow bit.
 * @param None.
 * @return None.
 */
inline void UART3_AutoBaudDetectOverflowReset(void);

/**
 * @ingroup uart3
 * @brief Checks if the UART3 receiver has received data and is ready to be read.
 * @param None.
 * @retval True - UART3 receiver FIFO has data
 * @retval False - UART3 receiver FIFO is empty
 */
bool UART3_IsRxReady(void);

/**
 * @ingroup uart3
 * @brief Checks if the UART3 transmitter is ready to accept a data byte.
 * @param None.
 * @retval True -  The UART3 transmitter FIFO has at least a one byte space
 * @retval False - The UART3 transmitter FIFO is full
 */
bool UART3_IsTxReady(void);

/**
 * @ingroup uart3
 * @brief Returns the status of the Transmit Shift Register (TSR).
 * @param None.
 * @retval True - Data completely shifted out from the TSR
 * @retval False - Data is present in Transmit FIFO and/or in TSR
 */
bool UART3_IsTxDone(void);

/**
 * @ingroup uart3
 * @brief Gets the error status of the last read byte. Call 
 *        this function before calling UART3_Read().
 * @pre Call UART3_RxEnable() to enable RX before calling this API.
 * @param None.
 * @return Status of the last read byte. See the uart3_status_t struct for more details.
 */
size_t UART3_ErrorGet(void);

/**
 * @ingroup uart3
 * @brief Reads the eight bits from the Receiver FIFO register.
 * @pre Check the transfer status to see if the receiver is not empty before calling this function. Check 
 *      UART3_IsRxReady() in if () before calling this API.
 * @param None.
 * @return 8-bit data from the RX FIFO register
 */
uint8_t UART3_Read(void);

/**
 * @ingroup uart3
 * @brief Writes a byte of data to the Transmitter FIFO register.
 * @pre Check the transfer status to see if the transmitter is not empty before calling this function. Check
 *      UART3_IsTxReady() in if () before calling this API.
 * @param txData  - Data byte to write to the TX FIFO
 * @return None.
 */
void UART3_Write(uint8_t txData);

/**
 * @ingroup uart3
 * @brief Calls the function upon UART3 framing error.
 * @param callbackHandler - Function pointer called when the framing error condition occurs
 * @return None.
 */
void UART3_FramingErrorCallbackRegister(void (* callbackHandler)(void));

/**
 * @ingroup uart3
 * @brief Calls the function upon UART3 overrun error.
 * @param callbackHandler - Function pointer called when the overrun error condition occurs
 * @return None.
 */
void UART3_OverrunErrorCallbackRegister(void (* callbackHandler)(void));

/**
 * @ingroup uart3
 * @brief Calls the function upon UART3 parity error.
 * @param callbackHandler - Function pointer called when the parity error condition occurs
 * @return None.
 */
void UART3_ParityErrorCallbackRegister(void (* callbackHandler)(void));

#ifdef __cplusplus  // Provide C++ Compatibility


    }

#endif

#endif  // UART3_H
