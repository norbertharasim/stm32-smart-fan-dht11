//
// Created by Admin on 9/28/2026.
//
#include "fan_controller.h"

static void Fan_ApplyCCR(FanController_t *fan, float duty_pct) {
    if (duty_pct < 0.0f) duty_pct = 0.0f;
    if (duty_pct > 100.0f) duty_pct = 100.0f;

    fan->current_duty_pct = duty_pct;
    uint32_t ccr_val = (uint32_t)((duty_pct / 100.0f) * (float)fan->arr_period);
    __HAL_TIM_SET_COMPARE(fan->htim, fan->channel, ccr_val);
}
void Fan_Init(FanController_t *fan, TIM_HandleTypeDef *htim, uint32_t channel, uint32_t arr_period) {
    if (!fan || !htim) return;

    fan->htim = htim;
    fan->channel = channel;
    fan->arr_period = arr_period;
    //default values

    fan->Kp = 8.0f;
    fan->Ki = 0.2f;
    fan->Kd = 1.0f;
    fan->setpoint = 30.0f;

    fan->out_min = 20.0f;
    fan->out_max = 100.0f;

    fan->integral = 0.0f;
    fan->prev_temp = 25.0f;
    fan->current_duty_pct = fan->out_min;

    HAL_TIM_PWM_Start(fan->htim, fan->channel);
    Fan_ApplyCCR(fan, fan->out_min);
}
void Fan_SetPID(FanController_t *fan, float Kp, float Ki, float Kd, float setpoint) {
   if (!fan) return;
    fan->Kp = Kp;
    fan->Ki = Ki;
    fan->Kd = Kd;
    fan->setpoint = setpoint;
}
void Fan_SetOutputLimits(FanController_t *fan, float out_min, float out_max) {
    if (!fan) return;
    fan->out_min = out_min;
    fan->out_max = out_max;
}
void FanSetTargetTemp(FanController_t *fan, float setpoint) {
    if (!fan) return;
    fan->setpoint = setpoint;
}

void Fan_PID_Update(FanController_t *fan, float current_temp, float dt) {
    if (!fan || dt <= 0.0f) return;
    float error = current_temp - fan->setpoint;

    float P = fan->Kp * error;

    fan->integral += error * dt;
    //Anti-windup
    if (fan->Ki > 0.0001f) {
        float max_integral = fan->out_max / fan->Ki;
        float min_integral = -fan->out_max / fan->Ki;

        if (fan->integral > max_integral) fan->integral = max_integral;
        else if (fan->integral < min_integral) fan->integral = min_integral;
    } else {
        fan->integral = 0.0f;
    }
    float I = fan->Ki * fan->integral;

    float d_temp = (current_temp - fan->prev_temp) / dt;
    float D = fan->Kd * d_temp;
    fan->prev_temp = current_temp;

    float output = P + I + D;

    if (output > fan->out_max) {
        output = fan->out_max;
    }
    else if (output < fan->out_min) {
        output = fan->out_min;
    }

    Fan_ApplyCCR(fan, output);
}
uint8_t Fan_GetDuty(const FanController_t * fan) {
    if (!fan) return 0;
    return (uint8_t)fan->current_duty_pct;
}