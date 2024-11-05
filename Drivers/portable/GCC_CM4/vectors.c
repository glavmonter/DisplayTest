#include "exception_handlers.h"

void Default_Handler (void) __attribute__((weak));

/* STM32F334 Specific Interrupts */
void WWDG_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void PVD_IRQHandler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI1_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_TSC_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI3_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_TX_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_RX0_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_RX1_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_SCE_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_TIM15_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void USART3_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM6_DAC1_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM7_DAC2_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void COMP2_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void COMP4_6_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void HRTIM1_Master_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void HRTIM1_TIMA_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void HRTIM1_TIMB_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void HRTIM1_TIMC_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void HRTIM1_TIMD_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void HRTIM1_TIME_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void HRTIM1_FLT_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void FPU_IRQHandler                     (void) __attribute__ ((weak, alias("Default_Handler")));


extern unsigned int __stack;

typedef void (*const pHandler)(void);

// The vector table.
// This relies on the linker script to place at correct location in memory.

pHandler __isr_vectors[] __attribute__ ((section(".isr_vector"),used)) =  {
        (pHandler) &__stack,                    // The initial stack pointer
        Reset_Handler,                          // The reset handler

        NMI_Handler,                            // The NMI handler
        HardFault_Handler,                      // The hard fault handler

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
        MemManage_Handler,                      // The MPU fault handler
        BusFault_Handler,                       // The bus fault handler
        UsageFault_Handler,                     // The usage fault handler
#else
        0, 0, 0,				                // Reserved
#endif
        0,                                      // Reserved
        0,                                      // Reserved
        0,                                      // Reserved
        0,                                      // Reserved
        SVC_Handler,                            // SVCall handler
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
        DebugMon_Handler,                       // Debug monitor handler
#else
        0,					                    // Reserved
#endif
        0,                                      // Reserved
        PendSV_Handler,                         // The PendSV handler
        SysTick_Handler,                        // The SysTick handler

        /* External interrupts */
        WWDG_IRQHandler,                /*  */
        PVD_IRQHandler,                 /*  */
        TAMP_STAMP_IRQHandler,          /*  */
        RTC_WKUP_IRQHandler,            /*  */
        FLASH_IRQHandler,               /*  */
        RCC_IRQHandler,                 /*  */
        EXTI0_IRQHandler,               /*  */
        EXTI1_IRQHandler,               /*  */
        EXTI2_TSC_IRQHandler,           /*  */
        EXTI3_IRQHandler,               /*  */
        EXTI4_IRQHandler,               /*  */
        DMA1_Channel1_IRQHandler,       /*  */
        DMA1_Channel2_IRQHandler,       /*  */
        DMA1_Channel3_IRQHandler,       /*  */
        DMA1_Channel4_IRQHandler,       /*  */
        DMA1_Channel5_IRQHandler,       /*  */
        DMA1_Channel6_IRQHandler,       /*  */
        DMA1_Channel7_IRQHandler,       /*  */
        ADC1_2_IRQHandler,              /*  */
        CAN_TX_IRQHandler,              /*  */
        CAN_RX0_IRQHandler,             /*  */
        CAN_RX1_IRQHandler,             /*  */
        CAN_SCE_IRQHandler,             /*  */
        EXTI9_5_IRQHandler,             /*  */
        TIM1_BRK_TIM15_IRQHandler,      /*  */
        TIM1_UP_TIM16_IRQHandler,       /*  */
        TIM1_TRG_COM_TIM17_IRQHandler,  /*  */
        TIM1_CC_IRQHandler,             /*  */
        TIM2_IRQHandler,                /*  */
        TIM3_IRQHandler,                /*  */
        0,                              /*  */
        I2C1_EV_IRQHandler,             /*  */
        I2C1_ER_IRQHandler,             /*  */
        0,                              /*  */
        0,                              /*  */
        SPI1_IRQHandler,                /*  */
        0,                              /*  */
        USART1_IRQHandler,              /*  */
        USART2_IRQHandler,              /*  */
        USART3_IRQHandler,              /*  */
        EXTI15_10_IRQHandler,           /*  */
        RTC_Alarm_IRQHandler,           /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        TIM6_DAC1_IRQHandler,           /*  */
        TIM7_DAC2_IRQHandler,           /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        COMP2_IRQHandler,               /*  */
        COMP4_6_IRQHandler,             /*  */
        0,                              /*  */
        HRTIM1_Master_IRQHandler,       /*  */
        HRTIM1_TIMA_IRQHandler,         /*  */
        HRTIM1_TIMB_IRQHandler,         /*  */
        HRTIM1_TIMC_IRQHandler,         /*  */
        HRTIM1_TIMD_IRQHandler,         /*  */
        HRTIM1_TIME_IRQHandler,         /*  */
        HRTIM1_FLT_IRQHandler,          /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        0,                              /*  */
        FPU_IRQHandler,                 /*  */
};

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
__attribute__ ((section(".after_vectors")))
void Default_Handler (void)
{
	while (1) ;
}
