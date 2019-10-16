#include <stdio.h>

#include "contiki.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "net/rime/rime.h"


/*---------------------------------------------------------------------------*/
PROCESS(ex2_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&ex2_unicast_process);
/*---------------------------------------------------------------------------*/
int wait = 0;

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message received from %d.%d: %s\n",
	 from->u8[0], from->u8[1], (char *)packetbuf_dataptr());

  leds_on(LEDS_GREEN);
  wait = 5;
}
/*---------------------------------------------------------------------------*/
static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{
  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  printf("unicast message sent to %d.%d: status %d num_tx %d\n",
    dest->u8[0], dest->u8[1], status, num_tx);
}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ex2_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)

  PROCESS_BEGIN();

  int rd = NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 20);
  if (rd == RADIO_RESULT_INVALID_VALUE) {
    printf("Unable to change channel\n");
  }

  rd = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, 0xFF);
  if (rd == RADIO_RESULT_INVALID_VALUE) {
    printf("Unable to change power\n");
  }

  unicast_open(&uc, 146, &unicast_callbacks);

  while(1) {
    static struct etimer et;
    linkaddr_t addr;

    etimer_set(&et, CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom("Va manger tes grands martin sale tchoin des plages de tes mères", 64);
    addr.u8[0] = 0x62;//0xb2;
    addr.u8[1] = 0x44;//0x88;
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
    }

    if (wait == 0) {
      leds_off(LEDS_GREEN);
    } else {
      wait--;
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
