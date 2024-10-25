#if defined(TDD_SOFTWARE) || defined(TDD_HARDWARE)

#include "AllTests.h"

int RunAllTests(void) {
    UNITY_BEGIN();
    
    RUN_LED_TESTS();
    RUN_1WIRE_TESTS();
    RUN_I2CHELPER_TESTS();
    RUN_MCP7941X_TESTS();
    RUN_ACCELEROMETER_TESTS();
    RUN_RUNNING_HOURS_TESTS();
    
    return UNITY_END();
}

#endif