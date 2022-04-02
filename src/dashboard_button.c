#include <Arduino.h>
#include <stdlib.h>
#include "dashboard_button.h"

dashboard_button* dashboard_button_init(
	uint8_t pin_input,
	uint8_t pin_led,
	void (*on_press)(void),
	void (*on_release)(void),
	void (*on_hold)(void)
){
	/* Allocate memory */
	dashboard_button* db = malloc(sizeof(dashboard_button));

	/* Store pin config */
	db->pin_input = pin_input;
	db->pin_led = pin_led;

	/* Store callbacks */
	db->on_press = on_press;
	db->on_release = on_release;
	db->on_hold = on_hold;

	/* Init state */
	db->led_state = LED_OFF;
	db->millis_start_press = 0UL;
	db->millis_start_hold = 0UL;

	/* Setup pins */
	pinMode(db->pin_led, OUTPUT);
	pinMode(db->pin_input, INPUT);
	analogWrite(db->pin_led, 0);

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
	uint16_t time;
	switch(db->led_state){
		case LED_OFF:
			analogWrite(db->pin_led, 0);
			break;
		case LED_ON:
			analogWrite(db->pin_led, 255);
			break;
		case LED_BLINK:
			analogWrite(db->pin_led, (millis() % 1000 < 500) * 255);
			break;
		case LED_STROBE:
			analogWrite(db->pin_led, (millis() % 200 < 100) * 255);
			break;
		case LED_BREATHE:
			time = (millis() % 512);
			if(time < 256){
				analogWrite(db->pin_led, time);
			}else{
				analogWrite(db->pin_led, 512 - time);
			}
			break;
	}

	/* Check if button is pressed */
	char is_pressed = digitalRead(db->pin_input);
	char was_pressed = (db->millis_start_press != 0);
	char was_hold = (db->millis_start_hold != 0);
	if(is_pressed){
		/* Pressed */
		if(was_pressed){
			/* Was already pressed */
			if(!was_hold && millis() - db->millis_start_press > DB_HOLD_TIME){	
				/* Hold */
				db->millis_start_hold = millis();
				if(db->on_hold != NULL) db->on_hold();
			}
		}else{
			/* First pressed */
			db->millis_start_press = millis();
			if(db->on_press != NULL) db->on_press();
		}

	}else{
		/* Not pressed */
		if(was_pressed){
			/* Released */
			if(db->on_release != NULL) db->on_release();
			db->millis_start_press = 0UL;
			db->millis_start_hold = 0UL;
		}
	}
}
