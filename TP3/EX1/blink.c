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

  PROCESS_BEGIN();

  printf("Clock second : %d\n", CLOCK_SECOND);

  while (1) {
    etimer_set(&et, 2 * CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    leds_toggle(LEDS_GREEN);
  }

  PROCESS_END();
}
