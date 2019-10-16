#include <stdio.h>

#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"

/*---------------------------------------------------------------------------*/
PROCESS(button_process, "Button process");
AUTOSTART_PROCESSES(&button_process);
/*---------------------------------------------------------------------------*/

int state = 0;

PROCESS_THREAD(button_process, ev, data) {
  PROCESS_BEGIN();
  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL((ev == sensors_event) && (data == &button_sensor));

    if (state) {
      leds_toggle(LEDS_BLUE);
    }

    state = !state;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
