#include <stdint.h>

#include "contiki.h"
#include "pwm.h"

#include "buzzer.h"

// Constants
#define BUZZER_TIMER     PWM_TIMER_1
#define BUZZER_PORT      ADC_SENSORS_PORT
#define BUZZER_FREQUENCY 100 // Hz

#define BUZZER_AB(pin) ((pin) == BUZZER_ADC1 ? PWM_TIMER_A : PWM_TIMER_B)

// Functions
int8_t buzzer_enable(uint8_t pin, uint8_t power) {
  return pwm_enable(BUZZER_FREQUENCY, power, 0, BUZZER_TIMER, BUZZER_AB(pin));
}

int8_t buzzer_disable(uint8_t pin) {
  return pwm_disable(BUZZER_TIMER, BUZZER_AB(pin), BUZZER_PORT, pin);
}

int8_t buzzer_on(uint8_t pin) {
  return pwm_start(BUZZER_TIMER, BUZZER_AB(pin), BUZZER_PORT, pin);
}

int8_t buzzer_off(uint8_t pin) {
  return pwm_stop(BUZZER_TIMER, BUZZER_AB(pin), BUZZER_PORT, pin, PWM_OFF_WHEN_STOP);
}
