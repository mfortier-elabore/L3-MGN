#ifdef TDD_SIM

#include "AllTests.h"

int RunAllTests(void) {
    UNITY_BEGIN();
    
    RUN_LED_TESTS();
    RUN_1WIRE_TESTS();
    RUN_I2CHELPER_TESTS();
    RUN_MCP7941X_TESTS();
    
    return UNITY_END();
}

#endif