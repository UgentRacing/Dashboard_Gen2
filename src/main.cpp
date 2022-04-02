#include <Arduino.h>
extern "C" {
	#include "slave_led.h"
	#include "dashboard_button.h"
}

/* Pin Definitions */
#define PIN_DEBUG 13
#define PIN_RESET 9
#define PIN_OUTPUT_ENABLE 5 /* Only for testing! */
#define PIN_IND_DATA 2
#define PIN_IND_DATA_CLOCK 8 /* Only for testing! */
#define PIN_IND_STORAGE_CLOCK 4
#define PIN_BUT_RTD_IN 17
#define PIN_BUT_RTD_LED 15
#define PIN_BUT_TS_IN 16
#define PIN_BUT_TS_LED 14

/* Declare vars */
dashboard_button* db_rtd;
dashboard_button* db_ts;

/* SETUP */
void setup() {
	/* Setup IO */
	pinMode(PIN_DEBUG, OUTPUT);
	digitalWrite(PIN_DEBUG, LOW);

	/* Setup buttons */
	db_rtd = dashboard_button_init(PIN_BUT_RTD_IN, PIN_BUT_RTD_LED);
	db_ts = dashboard_button_init(PIN_BUT_TS_IN, PIN_BUT_TS_LED);

	/* Test LED */
	dashboard_button_set_led(db_rtd, LED_BLINK);
}

/* LOOP */
void loop() {
	/* Check buttons */
	dashboard_button_update(db_rtd);
}

