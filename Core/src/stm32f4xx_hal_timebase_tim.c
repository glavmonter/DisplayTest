#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include <FreeRTOS.h>

TIM_HandleTypeDef        htim10;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
    RCC_ClkInitTypeDef    clkconfig;
    uint32_t              uwTimclock = 0U;

    uint32_t              uwPrescalerValue = 0U;
    uint32_t              pFLatency;
    HAL_StatusTypeDef     status;

    /* Enable TIM10 clock */
    __HAL_RCC_TIM10_CLK_ENABLE();

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

    /* Compute TIM10 clock */
    uwTimclock = HAL_RCC_GetPCLK2Freq();

    /* Compute the prescaler value to have TIM10 counter clock equal to 1MHz */
    uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);

    htim10.Instance = TIM10;

    /* Initialize TIMx peripheral as follow:
    + Period = [(TIM10CLK/1000) - 1]. to have a (1/1000) s time base.
    + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
    + ClockDivision = 0
    + Counter direction = Up
    */
    htim10.Init.Period = (1000000U / 1000U) - 1U;
    htim10.Init.Prescaler = uwPrescalerValue;
    htim10.Init.ClockDivision = 0;
    htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    status = HAL_TIM_Base_Init(&htim10);
    if (status == HAL_OK) {
        status = HAL_TIM_Base_Start_IT(&htim10);
        if (status == HAL_OK) {
            HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
            /* Configure the SysTick IRQ priority */
            if (TickPriority < (1UL << __NVIC_PRIO_BITS)) {
                /* Configure the TIM IRQ priority */
                HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, TickPriority, 0U);
                uwTickPrio = TickPriority;
            } else {
                status = HAL_ERROR;
            }
        }
    }
    return status;
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM10 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_SuspendTick(void) {
    __HAL_TIM_DISABLE_IT(&htim10, TIM_IT_UPDATE);
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by Enabling TIM10 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_ResumeTick(void) {
    __HAL_TIM_ENABLE_IT(&htim10, TIM_IT_UPDATE);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM10) {
        HAL_IncTick();
    }
}


void TIM1_UP_TIM10_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim10);
}
