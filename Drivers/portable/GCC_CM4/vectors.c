#include "exception_handlers.h"

void Default_Handler (void) __attribute__((weak));

/* STM32F401 Specific Interrupts */
void WWDG_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void PVD_IRQHandler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI1_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI3_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC_IRQHandler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM4_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SDIO_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM5_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI3_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void USART6_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void FPU_IRQHandler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI4_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));


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
        WWDG_IRQHandler,                    /* Window WatchDog              */
        PVD_IRQHandler,                     /* PVD through EXTI Line detection */
        TAMP_STAMP_IRQHandler,              /* Tamper and TimeStamps through the EXTI line */
        RTC_WKUP_IRQHandler,                /* RTC Wakeup through the EXTI line */
        FLASH_IRQHandler,                   /* FLASH                        */
        RCC_IRQHandler,                     /* RCC                          */
        EXTI0_IRQHandler,                   /* EXTI Line0                   */
        EXTI1_IRQHandler,                   /* EXTI Line1                   */
        EXTI2_IRQHandler,                   /* EXTI Line2                   */
        EXTI3_IRQHandler,                   /* EXTI Line3                   */
        EXTI4_IRQHandler,                   /* EXTI Line4                   */
        DMA1_Stream0_IRQHandler,            /* DMA1 Stream 0                */
        DMA1_Stream1_IRQHandler,            /* DMA1 Stream 1                */
        DMA1_Stream2_IRQHandler,            /* DMA1 Stream 2                */
        DMA1_Stream3_IRQHandler,            /* DMA1 Stream 3                */
        DMA1_Stream4_IRQHandler,            /* DMA1 Stream 4                */
        DMA1_Stream5_IRQHandler,            /* DMA1 Stream 5                */
        DMA1_Stream6_IRQHandler,            /* DMA1 Stream 6                */
        ADC_IRQHandler,                     /* ADC1                         */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        EXTI9_5_IRQHandler,                 /* External Line[9:5]s          */
        TIM1_BRK_TIM9_IRQHandler,           /* TIM1 Break and TIM9          */
        TIM1_UP_TIM10_IRQHandler,           /* TIM1 Update and TIM10        */
        TIM1_TRG_COM_TIM11_IRQHandler,      /* TIM1 Trigger and Commutation and TIM11 */
        TIM1_CC_IRQHandler,                 /* TIM1 Capture Compare         */
        TIM2_IRQHandler,                    /* TIM2                         */
        TIM3_IRQHandler,                    /* TIM3                         */
        TIM4_IRQHandler,                    /* TIM4                         */
        I2C1_EV_IRQHandler,                 /* I2C1 Event                   */
        I2C1_ER_IRQHandler,                 /* I2C1 Error                   */
        I2C2_EV_IRQHandler,                 /* I2C2 Event                   */
        I2C2_ER_IRQHandler,                 /* I2C2 Error                   */
        SPI1_IRQHandler,                    /* SPI1                         */
        SPI2_IRQHandler,                    /* SPI2                         */
        USART1_IRQHandler,                  /* USART1                       */
        USART2_IRQHandler,                  /* USART2                       */
        0,                                  /* Reserved                     */
        EXTI15_10_IRQHandler,               /* External Line[15:10]s        */
        RTC_Alarm_IRQHandler,               /* RTC Alarm (A and B) through EXTI Line */
        OTG_FS_WKUP_IRQHandler,             /* USB OTG FS Wakeup through EXTI line */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        DMA1_Stream7_IRQHandler,            /* DMA1 Stream7                 */
        0,                                  /* Reserved                     */
        SDIO_IRQHandler,                    /* SDIO                         */
        TIM5_IRQHandler,                    /* TIM5                         */
        SPI3_IRQHandler,                    /* SPI3                         */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        DMA2_Stream0_IRQHandler,            /* DMA2 Stream 0                */
        DMA2_Stream1_IRQHandler,            /* DMA2 Stream 1                */
        DMA2_Stream2_IRQHandler,            /* DMA2 Stream 2                */
        DMA2_Stream3_IRQHandler,            /* DMA2 Stream 3                */
        DMA2_Stream4_IRQHandler,            /* DMA2 Stream 4                */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        OTG_FS_IRQHandler,                  /* USB OTG FS                   */
        DMA2_Stream5_IRQHandler,            /* DMA2 Stream 5                */
        DMA2_Stream6_IRQHandler,            /* DMA2 Stream 6                */
        DMA2_Stream7_IRQHandler,            /* DMA2 Stream 7                */
        USART6_IRQHandler,                  /* USART6                       */
        I2C3_EV_IRQHandler,                 /* I2C3 event                   */
        I2C3_ER_IRQHandler,                 /* I2C3 error                   */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        FPU_IRQHandler,                     /* FPU                          */
        0,                                  /* Reserved                     */
        0,                                  /* Reserved                     */
        SPI4_IRQHandler,                    /* SPI4                         */
};

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
__attribute__ ((section(".after_vectors")))
void Default_Handler (void)
{
	while (1) ;
}
