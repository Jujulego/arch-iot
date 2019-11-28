#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"

#include "dev/adc-zoul.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/pwm.h"
#include "dev/rgb-bl-lcd.h"
#include "dev/zoul-sensors.h"

#include "net/netstack.h"
#include "net/rime/rime.h"

#include "buzzer.h"
#include "msg.h"

// Constants
#define CHANNEL 20
#define POWER 7

#define LCD_REFRESH  CLOCK_SECOND
#define MEAN_FACTOR  0.33
#define UNITS_COUNT  3

#define BUZZER_POWER 5
#define BUZZER_TIME  CLOCK_SECOND
#define COLD         10
#define HOT          30

// Enums
enum Unit {
  CELSIUS    = 0x00,
  KELVIN     = 0x01,
  FAHRENHEIT = 0x02
};

static const char* SYMBOLS[UNITS_COUNT] = {
  "\337C",
  "K",
  "\337F"
};

// Server process
PROCESS(server_process, "Server");
AUTOSTART_PROCESSES(&server_process);

// Variables
static struct etimer et;
static struct etimer et_buzzer;
static struct unicast_conn uc;

static uint8_t btn_pressed = 0;
static uint8_t force_refresh = 0;

static double mean = 0.0;
static enum Unit unit = CELSIUS;

// Utils
static double convert(double temp, enum Unit unit) {
  switch (unit) {
    case KELVIN:
      return temp + 273.15;

    case FAHRENHEIT:
      return (9.0 / 5.0) * temp + 32;

    case CELSIUS:
    default:
      return temp;
  }
}

static char* render(const char* format, ...) {
  int size = 0;
  char* str = NULL;
  va_list args;

  // Compute size
  va_start(args, format);
  size = vsprintf(str, format, args);
  va_end(args);

  if (size < 0) return NULL;

  // Allocate string
  size++;
  str = malloc(size * sizeof(char));

  if (str == NULL) return NULL;

  // Fill string
  va_start(args, format);
  size = vsprintf(str, format, args);
  va_end(args);

  if (size < 0) {
    free(str);
    return NULL;
  }

  return str;
}

// Callbacks
static void recv_uc(struct unicast_conn* c, const linkaddr_t* from) {
  if (IS_ADDRESS(*from, SENSOR)) {
    struct temp_msg msg;
    memcpy(&msg, packetbuf_dataptr(), sizeof(struct temp_msg));

    double pmean = mean;
    mean = (1 - MEAN_FACTOR) * mean + MEAN_FACTOR * msg.temp;
    printf("data:temperature:%d.%02d\n", (int) floor(mean), abs((int) round(mean * 100) % 100));

    if (mean >= HOT) {
      if (pmean < HOT) {
        buzzer_on(BUZZER_ADC1);
        etimer_set(&et_buzzer, BUZZER_TIME);
      }

      leds_on(LEDS_RED);
      leds_off(LEDS_BLUE);
    } else if (mean < COLD) {
      leds_off(LEDS_RED);
      leds_on(LEDS_BLUE);
    } else {
      leds_off(LEDS_RED);
      leds_off(LEDS_BLUE);
    }
  } else {
    printf("%x.%x: %s\n", from->u8[0], from->u8[1], (char*) packetbuf_dataptr());
  }
}

static void sent_uc(struct unicast_conn* c, int status, int num_tx) {
}

static void exit_handler() {
  lcd_clear_display();
  lcd_display(LCD_RGB_DISPLAY_OFF | LCD_RGB_DISPLAY_CURSOR_OFF);

  buzzer_disable(BUZZER_ADC1);

  unicast_close(&uc);
}

static const struct unicast_callbacks unicast_cbs = {recv_uc, sent_uc};

// Main
PROCESS_THREAD(server_process, ev, data) {
  PROCESS_EXITHANDLER(
    exit_handler();
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

  // Button config
  SENSORS_ACTIVATE(button_sensor);

  // LCD config
  SENSORS_ACTIVATE(rgb_bl_lcd);
  lcd_display(LCD_RGB_DISPLAY_ON | LCD_RGB_DISPLAY_CURSOR_ON);
  lcd_backlight_color(LCD_RGB_BLACK);

  // Buzzer config
  if (buzzer_enable(BUZZER_ADC1, 10) == BUZZER_ERROR) {
    printf("Unable to start buzzer\n");
  } else {
    buzzer_off(BUZZER_ADC1);
  }

  // Event loop
  etimer_set(&et, LCD_REFRESH);

  while (1) {
    PROCESS_WAIT_EVENT();

    // react to the button
    if ((ev == sensors_event) && (data == &button_sensor)) {
      btn_pressed = !btn_pressed;

      if (btn_pressed) {
        unit = (unit + 1) % UNITS_COUNT;
        force_refresh = 1;
      }
    }

    // buzzer
    if (etimer_expired(&et_buzzer)) {
      buzzer_off(BUZZER_ADC1);
    }

    // refresh lcd screen
    if (etimer_expired(&et) || force_refresh) {
      etimer_set(&et, LCD_REFRESH);
      force_refresh = 0;

      double temp = convert(mean, unit);
      int dec = (int) floor(temp);
      int pre = abs((int) round(temp * 100) % 100);

      char* str = render("%d,%02d %s", dec, pre, SYMBOLS[unit]);

      if (str != NULL) {
        lcd_clear_display();
        lcd_set_cursor(0, LCD_RGB_1ST_ROW);
        lcd_write(str);

        free(str);
      }
    }
  }

  PROCESS_END();
}
