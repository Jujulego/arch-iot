#include <stdint.h>

#include "contiki.h"
#include "pwm.h"

#include "beep.h"

// Constants
#define BEEP_TIMER     PWM_TIMER_1
#define BEEP_PORT      ADC_SENSORS_PORT
#define BEEP_FREQUENCY 100 // Hz
#define BEEP_DUTY      5   // %

#define BEEP_AB(pin) ((pin) == BEEP_ADC1 ? PWM_TIMER_A : PWM_TIMER_B)

// Functions
int8_t beep_enable(uint8_t pin, uint8_t power) {
  return pwm_enable(BEEP_FREQUENCY, power, 0, BEEP_TIMER, BEEP_AB(pin));
}

int8_t beep_disable(uint8_t pin) {
  return pwm_disable(BEEP_TIMER, BEEP_AB(pin), BEEP_PORT, pin);
}

int8_t beep_on(uint8_t pin) {
  return pwm_start(BEEP_TIMER, BEEP_AB(pin), BEEP_PORT, pin);
}

int8_t beep_off(uint8_t pin) {
  return pwm_stop(BEEP_TIMER, BEEP_AB(pin), BEEP_PORT, pin, PWM_OFF_WHEN_STOP);
}
