# Код printf: https://github.com/mpaland/printf

set(SOURCES printf.c)

# set(CMAKE_C_FLAGS_DEBUG ${CORE_OPTIMIZATION_OPTIONS})
# set(CMAKE_CXX_FLAGS_DEBUG ${CORE_OPTIMIZATION_OPTIONS})
add_library(printf-${CORE_POSTFIX} STATIC ${SOURCES})
