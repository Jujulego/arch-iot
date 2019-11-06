#include <math.h>
#include <stdio.h>
#include <string.h>

#include "contiki.h"

#include "dev/leds.h"

#include "net/netstack.h"
#include "net/rime/rime.h"

#include "msg.h"

// Constants
#define CHANNEL 20
#define POWER 7

#define LOOP_WAIT (CLOCK_SECOND / 2)
#define LED_WAIT 4
#define MEAN_FACTOR 0.8

// Server process
PROCESS(server_process, "Server");
AUTOSTART_PROCESSES(&server_process);

// Variables
static struct etimer et;
static struct unicast_conn uc;

static double mean = 0.0;

// Callbacks
static void recv_uc(struct unicast_conn* c, const linkaddr_t* from) {
  printf("%x.%x: ", from->u8[0], from->u8[1]);

  if ((from->u8[0] == 0x9f /*0x9c*/) && (from->u8[1] == 0xd9 /*0x75*/)) {
    struct temp_msg msg;
    memcpy(&msg, packetbuf_dataptr(), sizeof(struct temp_msg));

    mean = (1 - MEAN_FACTOR) * mean + MEAN_FACTOR * msg.temp;
    printf("%d mV => %d.%d °C\n", (int) round(msg.mv), (int) round(mean), (int) round(mean * 100) % 100);

    if (mean >= 30) {
      leds_on(LEDS_RED);
      leds_off(LEDS_BLUE);
    } else if (mean < 20) {
      leds_off(LEDS_RED);
      leds_on(LEDS_BLUE);
    } else {
      leds_off(LEDS_RED);
      leds_off(LEDS_BLUE);
    }
  } else {
    printf("%s\n", (char*) packetbuf_dataptr());
  }
}

static void sent_uc(struct unicast_conn* c, int status, int num_tx) {
}

static const struct unicast_callbacks unicast_cbs = {recv_uc, sent_uc};

// Main
PROCESS_THREAD(server_process, ev, data) {
  PROCESS_EXITHANDLER(
    unicast_close(&uc);
  )

  PROCESS_BEGIN();

  // Network config
  if (NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, CHANNEL) == RADIO_RESULT_INVALID_VALUE) {
    printf("Unable to change channel\n");
  }

  if (NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, POWER) == RADIO_RESULT_INVALID_VALUE) {
    printf("Unable to change power\n");
  }

  unicast_open(&uc, 146, &unicast_cbs);

  // Event loop
  while (1) {
    etimer_set(&et, LOOP_WAIT);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }

  PROCESS_END();
}
