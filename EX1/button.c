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
  SENSORS_ACTIVATE(button_sensor); //permet d'activer le bouton 

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL((ev == sensors_event) && (data == &button_sensor));
    //Quand on reçoit un événement et que c'est celui viens du bouton 

    if (state) { //si on est dans le state 
      leds_toggle(LEDS_BLUE);
      //change l'état de la led Bleue (On/Off) 
    }

    state = !state;
    //Change l'état du state cela permet de ne pas éteindre si on relache le bouton 
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
