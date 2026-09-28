//
// Created by Admin on 9/28/2026.
//

#ifndef STM32_DHT11_FAN_CONTROLLER_H
#define STM32_DHT11_FAN_CONTROLLER_H
#ifdef __cplusplus
extern "C" {
#endif //STM32_DHT11_FAN_CONTROLLER_H
#include "stm32f4xx_hal.h"
#include<stdint.h>

typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    uint32_t arr_period;
    float Kp;
    float Ki;
    float Kd;

    float out_min;
    float out_max;

    float setpoint;
    float integral;
    float prev_temp;
    float current_duty_pct;

} FanController_t;

void Fan_Init(FanController_t *fan, TIM_HandleTypeDef *htim, uint32_t channel, uint32_t arr_period);

void Fan_SetPID(FanController_t *fan, float Kp, float Ki, float Kd, float setpoint);

void Fan_SetOutputLimits(FanController_t *fan, float out_min, float out_max);

void Fan_SetTargetTemp(FanController_t *fan, float setpoint);

void Fan_PID_Update(FanController_t *fan, float current_temp, float dt);

uint8_t Fan_GetDuty(const FanController_t *fan);

#ifdef __cplusplus
}
#endif
#endif