#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <u8g2.h>
#include <U8g2lib.h>
#include <MUIU8g2.h>
#include <etl/vector.h>
#include <etl/debounce.h>
#include "u8g2task.h"
#include <stm32f4xx_hal.h>

#include <log_levels.h>
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL LOG_TAG_DISPLAY_LEVEL
#include <rtt_log.h>
static const char *TAG     = "   DISP";
#include <printf.h>

extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef hTimEncoder;
extern SPI_HandleTypeDef hspi2;

static void vDisplayTask(void *pvParameters);
static void vButtonTask(void *pvParameters);

constexpr size_t StatStackSize = configMINIMAL_STACK_SIZE * 2;
static StackType_t ucStatStack[StatStackSize];
static StaticTask_t xTCBTaskStat;
QueueHandle_t btnQueue;

enum Button {
    Push = 1 << 0,
    Next = 1 << 1,
    Last = 1 << 2,
    LongPush = 1 << 3
} ;

void InitU8GTask() {
    xTaskCreateStatic(vDisplayTask, "Disp", StatStackSize, nullptr, tskIDLE_PRIORITY, ucStatStack, &xTCBTaskStat);
    xTaskCreate(vButtonTask, "Btn", configMINIMAL_STACK_SIZE, nullptr, configMAX_PRIORITIES - 1, nullptr);
    btnQueue = xQueueCreate(1, sizeof(uint32_t));
}

static uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

static U8G2 display;
MUIU8G2 mui;

muif_t muif_list[] = {
    MUIF_U8G2_FONT_STYLE(0, u8g2_font_helvR08_tr),
    MUIF_U8G2_LABEL(),
    MUIF_BUTTON("BN", mui_u8g2_btn_exit_wm_fi)
};



fds_t fds_data[] = 
    MUI_FORM(1)
    MUI_STYLE(0)
    MUI_LABEL(5, 15, "Hello U8g2")
    MUI_XYT("BN", 64, 30, "Select Me");

void vDisplayTask(void *pvParameters) {
(void)pvParameters;
char str[16];

    RTT_LOGI(TAG, "Init");
    vTaskDelay(portMAX_DELAY);

    u8g2_Setup_ssd1309_i2c_128x64_noname0_f(display.getU8g2(), U8G2_R0, u8x8_byte_stm32_hw_i2c, u8x8_stm32_gpio_and_delay);

    display.setI2CAddress(0x3C);
    display.initDisplay();
    display.setPowerSave(0);

    mui.begin(display, fds_data, muif_list, sizeof(muif_list) / sizeof(muif_t));
    mui.gotoForm(1, 0);

    TickType_t t = xTaskGetTickCount();
    for (;;) {
        bool is_redraw = true;
        uint32_t btn_state;
        if (xQueueReceive(btnQueue, &btn_state, 0) == pdTRUE) {
            if (btn_state & (Push)) {
                mui.sendSelect();
                is_redraw = true;
            } else if (btn_state & (Next)) {
                mui.nextField();
                is_redraw = true;
            } else if (btn_state & (Last)) {
                mui.prevField();
                is_redraw = true;
            } else if (btn_state & LongPush) {
                mui.restoreForm();
            }
        }

        if (mui.isFormActive()) {
            if (is_redraw) {
                display.firstPage();
                do {
                    mui.draw();
                } while (display.nextPage());
                is_redraw = false;
            }
        } else {
            // Меню не активно, рисуем снеговика
            display.firstPage();
            do {
                display.clearBuffer();
                display.setDrawColor(2);
                display.drawFrame(0, 0, display.getWidth(), display.getHeight());
                
                display.setFont(u8g2_font_unifont_t_symbols);
                bool dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&hTimEncoder);
                u8g2_uint_t offset = 0;
                if (dir) {
                    offset = display.drawUTF8(5, 16, "Enc ↓");
                } else {
                    offset = display.drawUTF8(5, 16, "Enc ↑");
                }
                
                snprintf(str, sizeof(str), "%u", __HAL_TIM_GET_COUNTER(&hTimEncoder));
                display.drawStr(5 + offset, 16, str);

                display.setFont(u8g2_font_unifont_t_cyrillic);
                offset = display.drawUTF8(5, 60, "Снеговик:");
                display.setFont(u8g2_font_unifont_t_symbols);
                display.drawUTF8(5 + offset, 60, " ☃");
            } while (display.nextPage()); // ~ 30 ms on 400 kHz i2c and 110 ms on 100 kHz ms. Независимо от отрисовываемых данных
        }

        vTaskDelayUntil(&t, 10);
    }
}

//etl::debounce<70, 500> push_button;
etl::debounce<70, 500> user_push_button;


static void AD9833_WriteWord(uint16_t data)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, (uint8_t*)&data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

void AD9833_Init_1MHz(void)
{
    // Рассчитанное значение для 1 MHz
    uint32_t freq_word = 10737418; // ~1 MHz при 25 MHz MCLK

    uint16_t freq_reg_low  = (uint16_t)(0x4000 | (freq_word & 0x3FFF));
    uint16_t freq_reg_high = (uint16_t)(0x4000 | ((freq_word >> 14) & 0x3FFF));

    // Шаг 1: Сброс устройства, чтобы начать конфигурацию
    // RESET = 1: 0x2100 (Control register: B28=1 для записи 28 бит сразу, RESET=1)
    AD9833_WriteWord(0x2100);

    // Шаг 2: Записать регистры частоты FREQ0
    AD9833_WriteWord(freq_reg_low);
    AD9833_WriteWord(freq_reg_high);

    // Шаг 3: Включить генерацию синусоиды, снять RESET
    // Контрольный регистр: B28=1 уже не обязателен после записи, можно сбросить.
    // Для синусоиды и без RESET: 0x2000
    AD9833_WriteWord(0x2000);
    
    // Теперь AD9833 генерирует ~1 МГц синус.
}

void AD9833_Init_2MHz(void)
{
    // Рассчитанное значение для 2 MHz
    uint32_t freq_word = 21474836; // ~2 MHz при 25 MHz MCLK

    uint16_t freq_reg_low  = (uint16_t)(0x4000 | (freq_word & 0x3FFF));
    uint16_t freq_reg_high = (uint16_t)(0x4000 | ((freq_word >> 14) & 0x3FFF));

    // Сброс и установка режима записи 28 бит (B28=1) и RESET=1
    AD9833_WriteWord(0x2100);

    // Запись частоты в FREQ0
    AD9833_WriteWord(freq_reg_low);
    AD9833_WriteWord(freq_reg_high);

    // Снять RESET, установить вывод синусоиды
    AD9833_WriteWord(0x2000);
    // Теперь AD9833 генерирует ~2 МГц синус.
}


void vButtonTask(void *pvParameters) {
(void)pvParameters;
    vTaskDelay(200);
    AD9833_Init_1MHz();
    int freq = 1;

    for (;;) {
        // auto pin = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET; // true - нажата (кнопка на землю)
        // if (push_button.add(false)) {
        //     RTT_LOGI(TAG, "=============================");
        //     RTT_LOGI(TAG, "  Button    Set: %d", push_button.is_set());
        //     RTT_LOGI(TAG, "  Button   Held: %d", push_button.is_held());
        //     RTT_LOGI(TAG, "  Button Repeat: %d", push_button.is_repeating());

        //     if (push_button.is_held() && push_button.is_repeating()) {
        //         __HAL_TIM_SET_COUNTER(&hTimEncoder, 0);
        //     }

        //     if (push_button.is_set()) {
        //         uint32_t b = 1 << 0;
        //         xQueueOverwrite(btnQueue, &b);
        //     }

        //     if (push_button.is_held()) {
        //         uint32_t b = LongPush;
        //         xQueueOverwrite(btnQueue, &b);
        //     }
        // }

        auto pin = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET;
        if (user_push_button.add(pin)) {
            RTT_LOGI(TAG, "=============================");
            RTT_LOGI(TAG, "  Button    Set: %d", user_push_button.is_set());
            RTT_LOGI(TAG, "  Button   Held: %d", user_push_button.is_held());
            RTT_LOGI(TAG, "  Button Repeat: %d", user_push_button.is_repeating());
            
            if (user_push_button.is_held()) {
                switch (freq) {
                    case 1:
                        freq = 2;
                        AD9833_Init_2MHz();
                        break;
                    case 2:
                        freq = 1;
                        AD9833_Init_1MHz();
                        break;
                }
            }
        }
        vTaskDelay(1);
    }
}


uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    (void)arg_ptr;
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
