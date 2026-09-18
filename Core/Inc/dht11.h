//
// Created by Admin on 9/18/2026.
//

#ifndef STM32_DHT11_DHT11_H
#define STM32_DHT11_DHT11_H
#include "stm32f4xx_hal.h"
#include "main.h"

typedef struct {
    uint8_t temperature;
    uint8_t humidity;
} DHT11_Data_t;

void DHT11_Init(void);
uint8_t DHT11_Read(DHT11_Data_t *data);

#endif //STM32_DHT11_DHT11_H
