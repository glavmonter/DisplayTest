#include <cstring>
#include "main.h"
#include <SEGGER_RTT.h>
#include <printf.h>
#include "peripheral_init.h"

#ifdef __cplusplus
extern "C" {
#endif

void _putchar(char character) {
    SEGGER_RTT_PutChar(0, character);
}

#ifdef __cplusplus
}
#endif

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    if (CONFIG_LOG_MAXIMUM_LEVEL > 0) {
        SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    }

   for (;;) {
    __NOP();
   }
}
