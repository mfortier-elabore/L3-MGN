/* 
 * File: cartouche.h
 * Author: MFortier
 * Comments:
 * Revision history:
 *      Initial : 2023-04-03
 */

#ifndef ALL_TESTS_H
#define	ALL_TESTS_H

#include "unity.h"

void RUN_LED_TESTS(void);
void RUN_1WIRE_TESTS(void);
void RUN_I2CHELPER_TESTS(void);
void RUN_MCP7941X_TESTS(void);
void RUN_ACCELEROMETER_TESTS(void);
void RUN_RUNNING_HOURS_TESTS(void);

int RunAllTests(void);
#endif