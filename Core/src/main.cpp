#include <cstring>
#include <FreeRTOS.h>
#include <task.h>
#include <etl/string.h>
#include <etl/string_utilities.h>
#include "main.h"
#include <SEGGER_RTT.h>
#include "peripheral_init.h"

#include <log_levels.h>
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL LOG_TAG_MAIN_LEVEL
#include <rtt_log.h>
static const char *TAG     = "   MAIN";
static const char *TAG_RTS = "    RTS";

#include <printf.h>
#include "u8g2task.h"

#ifdef __cplusplus
extern "C" {
#endif

void _putchar(char character) {
    SEGGER_RTT_PutChar(0, character);
}

#ifdef __cplusplus
}
#endif

#if configGENERATE_RUN_TIME_STATS == 1
volatile unsigned long ulRunTimeStatsClock;

static void vStatTask(void *pvParameters);
constexpr size_t StatStackSize = configMINIMAL_STACK_SIZE * 2;
static StackType_t ucStatStack[StatStackSize];
static StaticTask_t xTCBTaskStat;

TIM_HandleTypeDef        htim11;
static void InitRuntimeStatisticTimer();
#endif


int main(void) {
    SystemClock_Config();
    if (CONFIG_LOG_MAXIMUM_LEVEL > 0) {
        DWT->CYCCNT = 0;
        DWT->CTRL |= 1;
        SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        rtt_log_set_vprintf([](const char *sFormat, va_list va) { return vprintf(sFormat, va); });
    }
    HAL_Init();
    MX_I2C1_Init();
    RTT_LOGI(TAG, "Hello");

#if configGENERATE_RUN_TIME_STATS == 1
    InitRuntimeStatisticTimer();
    xTaskCreateStatic(vStatTask, "Stat", StatStackSize, nullptr, configMAX_PRIORITIES - 2, ucStatStack, &xTCBTaskStat);
#endif

    InitU8GTask();
    vTaskStartScheduler();
    int cnt = 0;
    for (;;) {
        HAL_Delay(1000);
        printf("Ping: %d\n", cnt); cnt++;
    }
}


#if configGENERATE_RUN_TIME_STATS == 1
void vStatTask(void *pvParameters) {
(void)pvParameters;
    vTaskDelay(pdSEC_TO_TICKS(10));
    etl::string<configMAX_TASK_NAME_LEN + 1 + 2> task_name;

    uint32_t ulTotalRunTime;
    auto uxArraySize  = uxTaskGetNumberOfTasks();
    auto pxTaskStatusArray = new TaskStatus_t[uxArraySize];
    assert_param(pxTaskStatusArray);
    TickType_t tick = xTaskGetTickCount();
    for (;;) {
        auto free_heap = xPortGetFreeHeapSize();
        RTT_LOGD(TAG_RTS, "Free heap: %d", free_heap);
        RTT_LOGD(TAG_RTS, "== Name ===== [Prio] ==== Time ======== Handle ===");
        uxArraySize  = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
        ulTotalRunTime /= 100;
        if (ulTotalRunTime > 0) {
            for (UBaseType_t i = 0; i < uxArraySize; i++) {
                uint32_t percent = pxTaskStatusArray[i].ulRunTimeCounter / ulTotalRunTime;
                task_name = "[";
                task_name.append(pxTaskStatusArray[i].pcTaskName);
                task_name += "]";
                etl::pad_right(task_name, configMAX_TASK_NAME_LEN + 2, ' ');

                if (percent > 0) {
                    RTT_LOGD(TAG_RTS, "  %s [%lu] %10lu  %3lu%%   0x%08lX", task_name.c_str(), pxTaskStatusArray[i].uxBasePriority, pxTaskStatusArray[i].ulRunTimeCounter, percent, (uint32_t)pxTaskStatusArray[i].xHandle);
                } else {
                    RTT_LOGD(TAG_RTS, "  %s [%lu] %10lu   <1%%   0x%08lX", task_name.c_str(), pxTaskStatusArray[i].uxBasePriority, pxTaskStatusArray[i].ulRunTimeCounter, (uint32_t)pxTaskStatusArray[i].xHandle);
                }
            }
        }
        vTaskDelayUntil(&tick, pdSEC_TO_TICKS(30));
    }
}

void InitRuntimeStatisticTimer() {
    RCC_ClkInitTypeDef    clkconfig;
    uint32_t              uwTimclock = 0U;

    uint32_t              uwPrescalerValue = 0U;
    uint32_t              pFLatency;
    HAL_StatusTypeDef     status;

    /* Enable TIM11 clock */
    __HAL_RCC_TIM11_CLK_ENABLE();

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

    /* Compute TIM11 clock */
    uwTimclock = HAL_RCC_GetPCLK2Freq();

    /* Compute the prescaler value to have TIM11 counter clock equal to 1MHz */
    uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);

    htim11.Instance = TIM11;

    /* Initialize TIMx peripheral as follow:
    + Period = [(TIM11CLK/10000) - 1]. to have a (1/10000) s time base.
    + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
    + ClockDivision = 0
    + Counter direction = Up
    */
    htim11.Init.Period = (1000000U / 10000U) - 1U;
    htim11.Init.Prescaler = uwPrescalerValue;
    htim11.Init.ClockDivision = 0;
    htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    status = HAL_TIM_Base_Init(&htim11);
    if (status == HAL_OK) {
        status = HAL_TIM_Base_Start_IT(&htim11);
        if (status == HAL_OK) {
            HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0), 0U);
            HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
        }
    }
}

extern "C" void TIM1_TRG_COM_TIM11_IRQHandler() {
    if (TIM11->SR & TIM_SR_UIF) {
        ulRunTimeStatsClock++;
        TIM11->SR = 0;
    }
}
#endif

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
extern "C" void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                               StackType_t **ppxIdleTaskStackBuffer,
                                               uint32_t *pulIdleTaskStackSize )
{
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
extern "C" void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                                StackType_t **ppxTimerTaskStackBuffer,
                                                uint32_t *pulTimerTaskStackSize )
{
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


void vApplicationTickHook() {
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
(void)xTask;
static char taskname[configMAX_TASK_NAME_LEN + 1];
    strncpy(taskname, pcTaskName, configMAX_TASK_NAME_LEN);
    taskname[configMAX_TASK_NAME_LEN] = '\0';
    RTT_LOGE(TAG, "Stack Overflow: %s", taskname);
    assert_param(0);
    while (true) {}
}

extern "C" void vApplicationMallocFailedHook() {
    RTT_LOGE(TAG, "======MALLOC FAILED======");
    assert_param(0);
    while (true) {}
}
