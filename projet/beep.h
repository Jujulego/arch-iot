#pragma once

#include <stdint.h>

#include "dev/adc-zoul.h"
#include "pwm.h"

// Constants
#define BEEP_SUCCESS PWM_SUCCESS
#define BEEP_ERROR   PWM_ERROR
#define BEEP_ADC1    ADC_SENSORS_ADC1_PIN
#define BEEP_ADC3    ADC_SENSORS_ADC3_PIN

// Functions
int8_t beep_enable(uint8_t pin, uint8_t power);
int8_t beep_disable(uint8_t pin);

int8_t beep_on(uint8_t pin);
int8_t beep_off(uint8_t pin);
