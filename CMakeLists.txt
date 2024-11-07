SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake")
list(APPEND CMAKE_MODULE_PATH cmake)
include(cmake/functions.cmake)
include(cmake/GetGitVersion.cmake)

SET(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Core/STM32F401CCUX_FLASH.ld")

set(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")

SET(COMMON_FLAGS
        "-mcpu=cortex-m4 -mthumb ${FPU_FLAGS} -ffunction-sections -fdata-sections \
         -g -fno-common -Wall -Wextra -Wpointer-arith -fmessage-length=0 -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu11")
SET(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -std=gnu++14 -fno-rtti -fno-exceptions -fno-threadsafe-statics")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage")

project(DisplayTest C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(POSTFIX "d")
    # 0-NONE, 1-ERROR, 2-WARN, 3-INFO, 4-DEBUG, 5-VERBOSE
    add_definitions("-DCONFIG_LOG_MAXIMUM_LEVEL=5")
    add_definitions("-DDEBUG")
    add_definitions("-DUSE_ASSERT_INFO=2")
    add_definitions("-DUSE_FULL_ASSERT")
    add_definitions("-DFREERTOS_INFO") # INFO FreeRTOS Run time statistic
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(POSTFIX "")
    # 0-NONE, 1-ERROR, 2-WARN, 3-INFO, 4-DEBUG, 5-VERBOSE
    add_definitions("-DCONFIG_LOG_MAXIMUM_LEVEL=0")
    add_definitions("-DUSE_FULL_ASSERT")
else()
    message(WARNING "Unknown build type")
    message(FATAL_ERROR "Set CMAKE_BUILD_TYPE to Debug or Release and reconfigure")
endif()

# Заглушки newlib
set(NEWLIB_STUBS_SRC
        "Drivers/portable/GCC_CM4/newlib/_cxx.cpp"
        "Drivers/portable/GCC_CM4/newlib/_exit.c"
        "Drivers/portable/GCC_CM4/newlib/_syscalls.c"
        "Drivers/portable/GCC_CM4/newlib/_sbrk.c"
        )

# Векторы прерываний и начального старта
set(VECTORS_EXCEPTION_HANDLERS_SRC
        "Core/src/system_stm32f4xx.c"
        "Drivers/portable/GCC_CM4/exception_handlers.c"
        "Drivers/portable/GCC_CM4/initialize_hardware.c"
        "Drivers/portable/GCC_CM4/reset_hardware.c"
        "Drivers/portable/GCC_CM4/startup.c"
        "Drivers/portable/GCC_CM4/vectors.c"
        )

set(FREERTOS_INC
        "Middlewares/FreeRTOS/Source/include"
        "Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM4F"
        )
set(FREERTOS_ADDONS_INC
        "Middlewares/FreeRTOS-addons/cpp/include"
        "Middlewares/FreeRTOS-addons/c/include"
        )

set(COMMON_SRC ${VECTORS_EXCEPTION_HANDLERS_SRC} ${NEWLIB_STUBS_SRC} ${SELFTEST_SRC})

set(SPL_INC "Drivers/STM32F4xx_HAL_Driver/" "Drivers/STM32F4xx_HAL_Driver/inc")
set(CMSIS_INC "Drivers/CMSIS/include" "Drivers/CMSIS/Device/ST/STM32F4xx/include")
set(APP_INC "Core/inc")
set(SEGGER_INC "Middlewares/SEGGER")
set(LOGGING_INC "Middlewares/logging/include")
set(PRINTF_INC "Middlewares/printf")
set(MIDDLEWARES_INC "Middlewares")
set(FREERTOS "Middlewares/FreeRTOS")
set(FREERTOS_ADDONS_INC "Middlewares/FreeRTOS-addons/include")
set(RINGBUFFER_INC "Middlewares/RingBuffer")
set(U8G2_INC "Middlewares/u8g2")

include_directories(${CMSIS_INC})
include_directories(${SPL_INC})
include_directories(${APP_INC})
include_directories(${SEGGER_INC})
include_directories(${PRINTF_INC})
include_directories(${MIDDLEWARES_INC})
include_directories(${RINGBUFFER_INC})
include_directories(${FREERTOS}/include)
include_directories(${FREERTOS}/portable/GCC/ARM_CM4F)
include_directories(${FREERTOS_ADDONS_INC})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${LOGGING_INC})
include_directories(${U8G2_INC})

set(FREERTOS_HEAP_SIZE_KB  2)
set(FREERTOS_ADDONS_DEFINITIONS "-DCPP_FREERTOS_NO_CPP_STRINGS -DCPP_FREERTOS_NO_EXCEPTIONS")
set(COMMON_DEFINITIONS -DSTM32F401xC -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DHXTAL_VALUE=25000000 ${FREERTOS_ADDONS_DEFINITIONS})
add_definitions(${COMMON_DEFINITIONS})
add_definitions("-DPRINTF_INCLUDE_CONFIG_H")

add_subdirectory(Middlewares)
add_subdirectory(Drivers)
add_subdirectory(Middlewares/FreeRTOS)
add_subdirectory(Middlewares/FreeRTOS-addons)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")

set(PROJECT_SRC
    "Core/src/errors.c"
    "Core/src/main.cpp"
    "Core/src/peripheral_init.c"
    "Core/src/stm32f4xx_hal_msp.c"
    "Core/src/stm32f4xx_hal_timebase_tim.c"
    "Core/src/stm32f4xx_it.c"
    "Core/src/u8g2task.cpp"
)

get_git_version(VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION_SHORT VERSION_FULL)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Core/inc/version_app.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/include/version_app.h @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Core/inc/FreeRTOSConfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/include/FreeRTOSConfig.h @ONLY)

set(POSTFIX "${POSTFIX}")

set(PN "DisplayTest${POSTFIX}")
add_executable(${PN}.elf ${PROJECT_SRC} ${COMMON_SRC})
set_options(${PN} ${LINKER_SCRIPT})
target_link_libraries(${PN}.elf PRIVATE spl rtt printf freertos logging u8g2)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif() # Debug build
