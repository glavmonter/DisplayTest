SET(RTT_SRC SEGGER/SEGGER_RTT.c SEGGER/SEGGER_RTT_printf.c)
add_library(rtt STATIC ${RTT_SRC})

add_library(printf STATIC printf/printf.c)

SET(LOGGING_SRC logging/log_buffers.cpp
                logging/log_freertos.cpp
                logging/log.cpp)
add_library(logging ${LOGGING_SRC})


file(GLOB U8G2_SRCS u8g2/*.c)
file(GLOB U8G2_HDRS u8g2/*.h)
add_library(u8g2 ${U8G2_SRCS})
target_include_directories(u8g2 PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/u8g2>" "$<INSTALL_INTERFACE:include>")

file(GLOB U8G2_CPP_SRCS u8g2cpp/*.cpp)
file(GLOB U8G2_CPP_HDRS u8g2cpp/*.h)
add_library(u8g2cpp ${U8G2_CPP_SRCS})
target_include_directories(u8g2cpp PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/u8g2>" "$<INSTALL_INTERFACE:include>")
