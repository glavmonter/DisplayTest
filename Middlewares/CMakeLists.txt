SET(RTT_SRC SEGGER/SEGGER_RTT.c SEGGER/SEGGER_RTT_printf.c)
add_library(rtt STATIC ${RTT_SRC})

add_library(printf STATIC printf/printf.c)

SET(LOGGING_SRC logging/log_buffers.cpp
                logging/log_freertos.cpp
                logging/log.cpp)
add_library(logging ${LOGGING_SRC})
