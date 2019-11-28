#pragma once

#include <stdint.h>

#include "dev/adc-zoul.h"
#include "pwm.h"

// Constants
#define BUZZER_SUCCESS PWM_SUCCESS
#define BUZZER_ERROR   PWM_ERROR
#define BUZZER_ADC1    ADC_SENSORS_ADC1_PIN
#define BUZZER_ADC3    ADC_SENSORS_ADC3_PIN

// Functions
/*
 * pin : BUZZER_ADC1 ou BUZZER_ADC3
 * power : une valeur entre 0 et 100 (0 => pas de bruit, 100 bruit max)
 */
int8_t buzzer_enable(uint8_t pin, uint8_t power);
int8_t buzzer_disable(uint8_t pin);

int8_t buzzer_on(uint8_t pin);
int8_t buzzer_off(uint8_t pin);
