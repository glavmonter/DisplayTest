

set(FREERTOS_SRCS
        "timers.c"
        "queue.c"
        "list.c"
        "tasks.c"
        "event_groups.c"
        "croutine.c"
        "stream_buffer.c"
        "portable/GCC/ARM_CM4F/port.c"
        "portable/MemMang/heap_1.c"
        )

# set(CMAKE_C_FLAGS_DEBUG ${CORE_OPTIMIZATION_OPTIONS})
# set(CMAKE_CXX_FLAGS_DEBUG ${CORE_OPTIMIZATION_OPTIONS})
add_library(freertos STATIC ${FREERTOS_SRCS})
