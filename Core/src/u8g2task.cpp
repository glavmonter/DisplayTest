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
static uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

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


static U8G2 u8g2;
static MUIU8G2 mui;

const char *animals[] = { "Bird", "Bison", "Cat", "Crow", "Dog", "Elephant", "Fish", "Gnu", "Horse", "Koala", "Lion", "Mouse", "Owl", "Rabbit", "Spider", "Turtle", "Zebra" };

static uint8_t num_value = 0;
static uint8_t bar_value = 0;
static uint16_t animal_idx = 0;

static uint16_t animal_name_list_get_cnt(void *data) {
(void)data;
    return sizeof(animals)/sizeof(*animals);
}

static const char *animal_name_list_get_str(void *data, uint16_t index) {
(void)data;
    return animals[index];
}

static uint8_t mui_hrule(mui_t *ui, uint8_t msg) {
    if (msg == MUIF_MSG_DRAW) {
        u8g2.drawHLine(0, mui_get_y(ui), u8g2.getDisplayWidth());
    }
    return 0;
}

static uint8_t show_my_data(mui_t *ui, uint8_t msg) {
    if (msg == MUIF_MSG_DRAW) {
        char buffer[16];
        auto x = mui_get_x(ui);
        auto y = mui_get_y(ui);
        u8g2.drawStr(x + 5, y, "Num:");
        snprintf(buffer, sizeof(buffer), "%d", num_value);
        u8g2.drawStr(x + 50, y, buffer);

        u8g2.drawStr(x + 5, y + 12, "Bar:");
        snprintf(buffer, sizeof(buffer), "%d", bar_value);
        u8g2.drawStr(x + 50, y + 12, buffer);

        u8g2.drawStr(x + 5, y + 24, "Animal:");
        snprintf(buffer, sizeof(buffer), "%d=%s", animal_idx, animals[animal_idx]);
        u8g2.drawStr(x + 50, y + 24, buffer);
    }
    return 0;
}

const muif_t muif_list[] = {
    MUIF_U8G2_FONT_STYLE(0, u8g2_font_helvR08_tr),  // Обычный шрифт
    MUIF_U8G2_FONT_STYLE(1, u8g2_font_helvB08_tr),  // Жирный шрифт

    MUIF_RO("HR", mui_hrule),
    MUIF_U8G2_LABEL(),
    MUIF_RO("GP", mui_u8g2_goto_data),
    MUIF_BUTTON("GC", mui_u8g2_goto_form_w1_pi),

    MUIF_U8G2_U8_MIN_MAX("NV", &num_value, 0, 99, mui_u8g2_u8_min_max_wm_mud_pi),
    MUIF_U8G2_U8_MIN_MAX_STEP("NB", &bar_value, 0, 16, 1, MUI_MMS_2X_BAR, mui_u8g2_u8_bar_wm_mud_pf),
    MUIF_U8G2_U16_LIST("NA", &animal_idx, NULL, animal_name_list_get_str, animal_name_list_get_cnt, mui_u8g2_u16_list_line_wa_mud_pi),

    MUIF_RO("SH", show_my_data),

    MUIF_BUTTON("GO", mui_u8g2_btn_goto_wm_fi),
};

const fds_t fds_data[] = 
    MUI_FORM(1)
    MUI_STYLE(1)
    MUI_LABEL(5, 8, "SimpleRotary Lib")
    
    MUI_STYLE(0)
    MUI_XY("HR", 0, 11)
    MUI_DATA("GP",
        MUI_10 "Enter Data|"
        MUI_12 "Show Data")
    MUI_XYA("GC", 5, 24, 0)
    MUI_XYA("GC", 5, 36, 1)

    MUI_FORM(10)
    MUI_STYLE(1)
    MUI_LABEL(5, 8, "Enter Data")
    MUI_XY("HR", 0, 11)
    MUI_STYLE(0)
    MUI_LABEL(5, 23, "Num:")
    MUI_LABEL(5, 36, "Bar:")
    MUI_LABEL(5, 49, "Animal:")
    MUI_XY("NV", 50, 23)
    MUI_XY("NB", 50, 36)
    MUI_XYA("NA", 50, 49, 44)
    MUI_XYAT("GO", 114, 60, 1, " OK ")

    MUI_FORM(12)
    MUI_STYLE(1)
    MUI_LABEL(5, 8, "Show Data")
    MUI_XY("HR", 0, 11)
    MUI_STYLE(0)
    MUI_XY("SH", 0, 23)
    MUI_XYAT("GO", 114, 60, 1, " OK ")
;


void vDisplayTask(void *pvParameters) {
(void)pvParameters;
    RTT_LOGI(TAG, "Init");

    u8g2_Setup_ssd1309_i2c_128x64_noname0_f(u8g2.getU8g2(), U8G2_R0, u8x8_byte_stm32_hw_i2c, u8x8_stm32_gpio_and_delay);

    u8g2.setI2CAddress(0x3C);
    u8g2.initDisplay();
    u8g2.setPowerSave(0);

    mui.begin(u8g2, fds_data, muif_list, sizeof(muif_list) / sizeof(muif_t));
    mui.gotoForm(1, 0);

    TickType_t delay = xTaskGetTickCount();
    bool is_redraw = true;
    for (;;) {
        if (mui.isFormActive()) {
            if (is_redraw) {
                u8g2.firstPage();
                do {
                    mui.draw();
                } while (u8g2.nextPage());
                
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
