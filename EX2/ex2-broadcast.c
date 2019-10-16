#include <stdio.h>

#include "contiki.h"
#include "random.h"

#include "net/rime/rime.h"

#include "dev/button-sensor.h"
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
PROCESS(ex2_broadcast_process, "Ex2 Broadcast");
AUTOSTART_PROCESSES(&ex2_broadcast_process);
/*---------------------------------------------------------------------------*/
static void abc_recv(struct abc_conn *c) {
  printf("abc message received '%s'\n", (char *)packetbuf_dataptr());

  leds_on(LEDS_YELLOW);
//  clock_wait(1 * CLOCK_SECOND);
//  leds_off(LEDS_YELLOW);
}

static const struct abc_callbacks abc_call = {abc_recv};
static struct abc_conn abc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ex2_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(abc_close(&abc);)

  PROCESS_BEGIN();

  abc_open(&abc, 128, &abc_call);

  while(1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom("angelique.petillon@gmail.com", 30);
    abc_send(&abc);
    printf("abc message sent\n");

    leds_off(LEDS_YELLOW);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
