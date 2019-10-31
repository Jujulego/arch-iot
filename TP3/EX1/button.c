#include <stdio.h>

#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"

// Process
PROCESS(blink_process, "Blink");
AUTOSTART_PROCESSES(&blink_process);

// Blink !
PROCESS_THREAD(blink_process, ev, data) {
  static struct etimer et;
  static int pressed = 0;

  PROCESS_BEGIN();

  printf("Clock second : %d\n", CLOCK_SECOND);

  while (1) {
    PROCESS_WAIT_EVENT();
    if (etimer_expired(&et)) { // Timer
      leds_on(LEDS_GREEN);
    }

    if ((ev == sensors_event) && (data == &button_sensor)) { // Button
      if (!pressed) {
        leds_off(LEDS_GREEN);
        etimer_set(&et, 2 * CLOCK_SECOND);
      }

      pressed = !pressed;
    }
  }

  PROCESS_END();
}
