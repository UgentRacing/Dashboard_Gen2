#include <Arduino.h>
#include <stdlib.h>
#include "dashboard_button.h"

dashboard_button* dashboard_button_init(
	uint8_t pin_input,
	uint8_t pin_led
){
	/* Allocate memory */
	dashboard_button* db = malloc(sizeof(dashboard_button));

	/* Store pin config */
	db->pin_input = pin_input;
	db->pin_led = pin_led;

	/* Init state */
	db->led_state = LED_OFF;

	/* Setup pins */
	pinMode(db->pin_led, OUTPUT);
	pinMode(db->pin_input, INPUT);
	digitalWrite(db->pin_led, LOW);

	return db;
}

void dashboard_button_free(dashboard_button* db){
	/* Free all memory */
	free(db);
}

void dashboard_button_set_led(dashboard_button* db, led_state_t ls){
	/* Store new state */
	db->led_state = ls;
}

void dashboard_button_update(dashboard_button* db){
	/* Update LED */
	if(db->led_state == LED_OFF){
		digitalWrite(db->pin_led, LOW);
	}else if(db->led_state == LED_ON){
		digitalWrite(db->pin_led, HIGH);
	}else if(db->led_state == LED_BLINK){
		unsigned long time = millis();
		if(time % 1000 < 500){
			digitalWrite(db->pin_led, HIGH);
		}else{
			digitalWrite(db->pin_led, LOW);
		}
	}

	/* Check if button is pressed */
	char pressed = digitalRead(db->pin_input);
	if(pressed){
		/* Set debug LED */
		digitalWrite(13, HIGH);
	}else{
		digitalWrite(13, LOW);
	}
}
