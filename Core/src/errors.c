/**
  * \file    errors.cpp
  * \version V1.0.0
  * \brief Asserts
  */
#include <SEGGER_RTT.h>
#include <cmsis_compiler.h>
#include "errors.h"


#if USE_ASSERT_INFO == 1
void assert_failed(uint8_t *file, uint32_t line)
{
    SEGGER_RTT_printf(0, "assert_param() failed: file \"%s\", line %d\n", file, line);
    abort();
}
#elif USE_ASSERT_INFO == 2
void assert_failed(uint8_t* file, uint32_t line, const uint8_t* expr) {
    SEGGER_RTT_printf(0, "assert_param() failed: file \"%s\", line %d, expr \"%s\"\n", file, line, expr);
    abort();
}
#endif


void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}
