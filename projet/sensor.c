#include <math.h>
#include <stdio.h>

#include "contiki.h"

#include "dev/adc-zoul.h"
#include "dev/leds.h"
#include "dev/zoul-sensors.h"

#include "net/netstack.h"
#include "net/rime/rime.h"

#include "msg.h"

// Constants
#define B 4250
#define R0 100000

#define CHANNEL 20
#define POWER 7

#define LOOP_WAIT (CLOCK_SECOND / 2)
#define LED_WAIT 4

// Sensor process
PROCESS(sensor_process, "Sensor");
AUTOSTART_PROCESSES(&sensor_process);

static int wait = 0;

// Callbacks
static void recv_uc(struct unicast_conn* c, const linkaddr_t* from) {
  printf("unicast message from %x.%x: %s\n", from->u8[0], from->u8[1], (char*) packetbuf_dataptr());
}

static void sent_uc(struct unicast_conn* c, int status, int num_tx) {
  if (status == 0) {
    leds_on(LEDS_GREEN);
    leds_off(LEDS_RED);
  } else {
    leds_off(LEDS_GREEN);
    leds_on(LEDS_RED);
  }

  wait = LED_WAIT;
}

// Variables
static struct etimer et;

static struct unicast_conn uc;
static const struct unicast_callbacks unicast_cbs = {recv_uc, sent_uc};

// Main
PROCESS_THREAD(sensor_process, ev, data) {
  PROCESS_EXITHANDLER(
    unicast_close(&uc);
  )

  PROCESS_BEGIN();

  // Sensor config
  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC1);

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
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&et)) {
      // Read sensor
      double vdd = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);

      double mv = adc_zoul.value(ZOUL_SENSORS_ADC1) / 10.0;
      double r = R0 * ((vdd / mv) - 1.0);
      double t = 1.0 / (log(r / R0) / B + 1 / 298.15) - 273.15;

      printf("ADC1 = %d mv => %d °C\n", (int) round(mv), (int) round(t));

      // Send data
      struct temp_msg msg;
      msg.mv = mv;
      msg.temp = t;

      linkaddr_t server;
      SET_ADDRESS(server, SERVER);

      packetbuf_copyfrom(&msg, sizeof(struct temp_msg));
      unicast_send(&uc, &server);

      // Manage led
      if (wait > 0) {
        wait--;

        if (wait == 0) {
            leds_off(LEDS_GREEN);
            leds_off(LEDS_RED);
        }
      }
    }
  }

  PROCESS_END();
}
