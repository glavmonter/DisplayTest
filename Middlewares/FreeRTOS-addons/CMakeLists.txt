set(FREERTOS_ADDONS_SRCS
        ccondition_variable.cpp
        cevent_groups.cpp
        cmem_pool.cpp
        cmutex.cpp
        cqueue.cpp
        cread_write_lock.cpp
        csemaphore.cpp
        ctasklet.cpp
        cthread.cpp
        ctickhook.cpp
        ctimer.cpp
        cworkqueue.cpp
)

add_library(freertos-addons-${CORE_POSTFIX} STATIC ${FREERTOS_ADDONS_SRCS})
