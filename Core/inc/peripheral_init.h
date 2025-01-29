#ifndef _PERIPHERAL_INIT_H
#define PERIPHERAL_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

void SystemClock_Config(void);

void MX_GPIO_Init(void);
void MX_I2C1_Init(void);
void InitEncoderTimer(void);
void MX_SPI2_Init(void);

#ifdef __cplusplus
}
#endif

#endif // PERIPHERAL_INIT_H
