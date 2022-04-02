#include <Arduino.h>
extern "C" {
	#include "slave_led.h"
}

/* Pin Definitions */
#define PIN_DEBUG 13
#define PIN_RESET 9
#define PIN_OUTPUT_ENABLE 5 /* Only for testing! */
#define PIN_IND_DATA 2
#define PIN_IND_DATA_CLOCK 8 /* Only for testing! */
#define PIN_IND_STORAGE_CLOCK 4

void setup() {
	/* Setup IO */
	pinMode(PIN_DEBUG, OUTPUT);
	digitalWrite(PIN_DEBUG, LOW);

	/* Setup indicator slave */
	slave_led* slave_ind = slave_led_init(
		PIN_RESET,
		PIN_OUTPUT_ENABLE,
		PIN_IND_DATA,
		PIN_IND_DATA_CLOCK,
		PIN_IND_STORAGE_CLOCK
	);
	slave_led_reset(slave_ind);

	/* Set indicator on */
	slave_led_set(slave_ind, 0, COLOR_RED_GREEN);
	slave_led_set(slave_ind, 2, COLOR_RED_GREEN);
	slave_led_set(slave_ind, 4, COLOR_RED_GREEN);
	slave_led_set(slave_ind, 6, COLOR_RED_GREEN);
	slave_led_show(slave_ind);
}

void loop() {
	/* Blink LED */
	digitalWrite(PIN_DEBUG, HIGH);
	delay(200);
	digitalWrite(PIN_DEBUG, LOW);
	delay(200);
}
