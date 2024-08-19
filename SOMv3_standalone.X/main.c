#define F_CPU 1000000UL

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "mcc_generated_files/i2c_host/i2c_host_interface.h"

#include "TDD/unity.h"

void test_function1() {
    // Simple demo of working test
    TEST_ASSERT_TRUE(1);
}

void test_function2() {
    // Simple demo of failing test
    TEST_ASSERT_FALSE(1);
}

int run_unit_tests(void)
{
    UnityBegin("main.c");
    RUN_TEST(test_function1);
    RUN_TEST(test_function2);
    UnityEnd();
    return 0;   
}

int main(void)
{
    // Initialize drivers from MCC
    SYSTEM_Initialize();
 
    run_unit_tests();
}

void setUp (void) {} 
void tearDown (void) {}
