#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <u8g2.h>
#include <U8g2lib.h>
#include <MUIU8g2.h>
#include <etl/vector.h>
#include <etl/math.h>
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
    xTaskCreateStatic(vDisplayTask, "Disp", StatStackSize, nullptr, tskIDLE_PRIORITY + 1, ucStatStack, &xTCBTaskStat);
    xTaskCreate(vButtonTask, "Btn", configMINIMAL_STACK_SIZE, nullptr, configMAX_PRIORITIES - 1, nullptr);
    btnQueue = xQueueCreate(1, sizeof(uint32_t));
}

static uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t mui_draw_current_timer(mui_t *ui, uint8_t msg);
static uint8_t mui_start_current_timer(mui_t *ui, uint8_t msg);
static uint8_t mui_stop_current_timer(mui_t *ui, uint8_t msg);

static U8G2 display;
MUIU8G2 mui;

const fds_t fds_data[] = 
    MUI_FORM(1)
    MUI_AUX("SO")                           // Invisible field which will receive a form change event to stop the timer.
    MUI_STYLE(0)
    MUI_LABEL(5, 12, "Stopwatch")
    MUI_XY("CT", 5, 24)                     // Поле, где отображается позиция таймера stopwatch
    MUI_XYAT("GO", 20, 36, 2, " Start ")    // Переход на форму 2

    MUI_FORM(2)
    MUI_AUX("ST")
    MUI_STYLE(0)
    MUI_LABEL(5, 12, "Stopwatch")
    MUI_XY("CT", 5, 24)
    MUI_XYAT("GO", 20, 36, 1, " Stop ")
;

const muif_t muif_list[] = {
    MUIF_U8G2_FONT_STYLE(0, u8g2_font_helvR08_tr),

    MUIF_RO("CT", mui_draw_current_timer),
    MUIF_RO("ST", mui_start_current_timer),
    MUIF_RO("SO", mui_stop_current_timer),
    MUIF_BUTTON("GO", mui_u8g2_btn_goto_wm_fi),
    MUIF_LABEL(mui_u8g2_draw_text)
};

TickType_t stop_watch_timer = 0;   // Таймер в 1/100 секунды
TickType_t stop_watch_millis = 0;
bool is_stop_watch_running = 1;

/**
 * @brief Отрисовать текущее значение таймера
 * 
 * @param ui 
 * @param msg 
 * @return uint8_t 
 */
uint8_t mui_draw_current_timer(mui_t *ui, uint8_t msg) {
char buffer[8];
    if (msg == MUIF_MSG_DRAW) {
        snprintf(buffer, sizeof(buffer), "%d.%d", stop_watch_timer/1000, (stop_watch_timer/10)%100);
        display.drawStr(mui_get_x(ui), mui_get_y(ui), buffer);
    }
    return 0;
}

uint8_t mui_start_current_timer(mui_t *ui, uint8_t msg) {
(void)ui;
    if (msg == MUIF_MSG_FORM_START) {
        is_stop_watch_running = true;
        stop_watch_millis = xTaskGetTickCount();
        stop_watch_timer = 0;
    }
    return 0;
}

uint8_t mui_stop_current_timer(mui_t *ui, uint8_t msg) {
(void)ui;
    if (msg == MUIF_MSG_FORM_START) {
        is_stop_watch_running = false;
    }
    return false;
}

void vDisplayTask(void *pvParameters) {
(void)pvParameters;
    RTT_LOGI(TAG, "Init");

    u8g2_Setup_ssd1309_i2c_128x64_noname0_f(display.getU8g2(), U8G2_R0, u8x8_byte_stm32_hw_i2c, u8x8_stm32_gpio_and_delay);

    display.setI2CAddress(0x3C);
    display.initDisplay();
    display.setPowerSave(0);

    mui.begin(display, fds_data, muif_list, sizeof(muif_list) / sizeof(muif_t));
    mui.gotoForm(1, 0);

    TickType_t delay = xTaskGetTickCount();
    bool is_redraw = true;
    for (;;) {
        if (mui.isFormActive()) {
            if (is_redraw) {
                display.firstPage();
                do {
                    mui.draw();
                } while (display.nextPage());
                
                is_redraw = false;
            }

            uint32_t btn_state;
            if (xQueueReceive(btnQueue, &btn_state, 0) == pdTRUE) {
                if (btn_state & (Push)) {
                    RTT_LOGI(TAG, "MUI: sendSelect");
                    mui.sendSelect();
                    is_redraw = true;
                } else if (btn_state & (Next)) {
                    RTT_LOGI(TAG, "MUI: nextField");
                    mui.nextField();
                    is_redraw = true;
                } else if (btn_state & (Last)) {
                    RTT_LOGI(TAG, "MUI: prevField");
                    mui.prevField();
                    is_redraw = true;
                }
            }

            if (is_stop_watch_running) {
                stop_watch_timer = xTaskGetTickCount() - stop_watch_millis;
                is_redraw = true;
            }
        } else {
            mui.gotoForm(1, 0); // Форма 1 всегда отображается.
        }

        vTaskDelayUntil(&delay, 50);
    }
}


static etl::debounce<70/2, 500/2> push_button;
#define ENCODER_UPDATE_RATE     (50)

void vButtonTask(void *pvParameters) {
(void)pvParameters;
    vTaskDelay(200);
    
    uint32_t encoderCheck = 50;
    uint16_t lastCnt = __HAL_TIM_GET_COUNTER(&hTimEncoder);
    TickType_t lastTick = xTaskGetTickCount();
    for (;;) {
        auto pin = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET; // true - нажата (кнопка на землю)
        if (push_button.add(pin)) {
            RTT_LOGI(TAG, "=============================");
            RTT_LOGI(TAG, "  Button    Set: %d", push_button.is_set());
            RTT_LOGI(TAG, "  Button   Held: %d", push_button.is_held());
            RTT_LOGI(TAG, "  Button Repeat: %d", push_button.is_repeating());

            if (push_button.is_set()) {
                uint32_t b = Push;
                xQueueOverwrite(btnQueue, &b);
            }
        }

        if (encoderCheck == 0) {
            uint16_t currentCnt = __HAL_TIM_GET_COUNTER(&hTimEncoder);
            if (currentCnt != lastCnt) {
                uint32_t b;
                auto delta = etl::absolute_unsigned(currentCnt - lastCnt);
                RTT_LOGD(TAG, " Enc delta: %d, cnt: %d", delta, currentCnt);
                if (delta >= 2) {
                    if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&hTimEncoder)) {
                        b = Next;
                    } else {
                        b = Last;
                    }
                    xQueueOverwrite(btnQueue, &b);
                }
                lastCnt = currentCnt;
            }
            encoderCheck = ENCODER_UPDATE_RATE;
        }

        encoderCheck--;
        vTaskDelayUntil(&lastTick, 2);
    }
}


uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    (void)arg_ptr;
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            RTT_LOGV(TAG, "U8X8_MSG_GPIO_AND_DELAY_INIT");
            break;

        case U8X8_MSG_DELAY_MILLI:
            RTT_LOGV(TAG, "U8X8_MSG_DELAY_MILLI: %d", arg_int);
            vTaskDelay(arg_int);
            break;

        case U8X8_MSG_GPIO_DC:
            RTT_LOGV(TAG, "U8X8_MSG_GPIO_DC");
            break;

        case U8X8_MSG_GPIO_RESET:
            RTT_LOGV(TAG, "U8X8_MSG_GPIO_RESET");
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
            RTT_LOGV(TAG, "Byte send. arg_int %d", arg_int);
            while (arg_int > 0) {
                buffer.push_back(*pData);
                pData++;
                arg_int--;
            }
            break;
        
        case U8X8_MSG_BYTE_START_TRANSFER:
            RTT_LOGV(TAG, "Start transfer");
            buffer.clear();
            break;
        
        case U8X8_MSG_BYTE_END_TRANSFER:
            RTT_LOGV(TAG, "End transfer and send %d bytes", buffer.size());
            if (HAL_I2C_Master_Transmit(&hi2c1, u8x8_GetI2CAddress(u8x8) << 1, buffer.data(), buffer.size(), 100) != HAL_OK) {
                RTT_LOGE(TAG, "Error transfer");
                return 0;
            }
            break;
    }
    return 1;
}
