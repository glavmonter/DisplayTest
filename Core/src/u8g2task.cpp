#include <FreeRTOS.h>
#include <task.h>
#include <u8g2.h>
#include <etl/vector.h>
#include "u8g2task.h"
#include <stm32f4xx_hal.h>

#include <log_levels.h>
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL LOG_TAG_DISPLAY_LEVEL
#include <rtt_log.h>
static const char *TAG     = "   DISP";

extern I2C_HandleTypeDef hi2c1;

static void vDisplayTask(void *pvParameters);

constexpr size_t StatStackSize = configMINIMAL_STACK_SIZE * 2;
static StackType_t ucStatStack[StatStackSize];
static StaticTask_t xTCBTaskStat;

void InitU8GTask() {
    xTaskCreateStatic(vDisplayTask, "Disp", StatStackSize, nullptr, configMAX_PRIORITIES - 1, ucStatStack, &xTCBTaskStat);
}

static u8g2_t u8g2;

static uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

void vDisplayTask(void *pvParameters) {
(void)pvParameters;
    RTT_LOGI(TAG, "Init");
    u8g2_Setup_ssd1309_i2c_128x64_noname0_f(&u8g2, U8G2_R0, u8x8_byte_stm32_hw_i2c, u8x8_stm32_gpio_and_delay);
    u8g2_SetI2CAddress(&u8g2, 0x3C);    
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    for (;;) {
        u8g2_FirstPage(&u8g2);
        do {
            u8g2_ClearBuffer(&u8g2);
            u8g2_SetDrawColor(&u8g2, 2);
            u8g2_DrawFrame(&u8g2, 0, 0, u8g2.width, u8g2.height);

            u8g2_SetFont(&u8g2, u8g2_font_unifont_t_cyrillic);
            auto offset = u8g2_DrawUTF8(&u8g2, 5, 36, "Снеговик:");
            u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
            u8g2_DrawUTF8(&u8g2, 5 + offset, 36, " ☃");
        } while (u8g2_NextPage(&u8g2));

        vTaskDelay(50);
    }
}


uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            RTT_LOGD(TAG, "U8X8_MSG_GPIO_AND_DELAY_INIT");
            break;

        case U8X8_MSG_DELAY_MILLI:
            RTT_LOGD(TAG, "U8X8_MSG_DELAY_MILLI: %d", arg_int);
            vTaskDelay(arg_int);
            break;

        case U8X8_MSG_GPIO_DC:
            RTT_LOGD(TAG, "U8X8_MSG_GPIO_DC");
            break;

        case U8X8_MSG_GPIO_RESET:
            RTT_LOGD(TAG, "U8X8_MSG_GPIO_RESET");
            break;

        default:
            RTT_LOGW(TAG, "Unhandled GPIO and Delay: %d", msg);
            u8x8_SetGPIOResult(u8x8, 1);
            break;
    }

    return 1;
}


uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
/* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
static etl::vector<uint8_t, 32> buffer;
uint8_t *pData = static_cast<uint8_t *>(arg_ptr);

    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            RTT_LOGD(TAG, "Byte send. arg_int %d", arg_int);
            while (arg_int > 0) {
                buffer.push_back(*pData);
                pData++;
                arg_int--;
            }
            break;
        
        case U8X8_MSG_BYTE_START_TRANSFER:
            RTT_LOGD(TAG, "Start transfer");
            buffer.clear();
            break;
        
        case U8X8_MSG_BYTE_END_TRANSFER:
            RTT_LOGD(TAG, "End transfer and send %d bytes", buffer.size());
            if (HAL_I2C_Master_Transmit(&hi2c1, u8x8_GetI2CAddress(u8x8) << 1, buffer.data(), buffer.size(), 100) != HAL_OK) {
                RTT_LOGE(TAG, "Error transfer");
                return 0;
            }
            break;
    }
    return 1;
}
