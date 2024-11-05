#ifndef _PERIPHERAL_INIT_H
#define PERIPHERAL_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

void SystemClock_Config(void);

void MX_GPIO_Init(void);
void MX_ADC1_Init(void);
void MX_HRTIM1_Init(void);
void MX_HRTIM1_LL_Init(void);
void MX_I2C1_Init(void);
void MX_IWDG_Init(void);
void MX_SPI1_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif // PERIPHERAL_INIT_H
