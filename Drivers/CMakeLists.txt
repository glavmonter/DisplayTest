set(SPL_SRC
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_adc.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_adc_ex.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_cortex.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_dma.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_exti.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_flash_ex.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_flash.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_gpio.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_i2c_ex.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_i2c.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_iwdg.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_pwr_ex.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_pwr.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_rcc_ex.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_rcc.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_spi_ex.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_spi.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_tim_ex.c"
        "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_tim.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_uart_ex.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_hal_uart.c"

        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_adc.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_dma.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_exti.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_gpio.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_i2c.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_rcc.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_spi.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_usart.c"
        # "STM32F4xx_HAL_Driver/src/stm32f4xx_ll_utils.c"
)

add_library(spl STATIC ${SPL_SRC})
