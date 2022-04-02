#include <Arduino.h>
#include <stdlib.h>
#include "slave_led.h"

slave_led* slave_led_init(
	uint8_t pin_reset,
	uint8_t pin_output_enable,
	uint8_t pin_data,
	uint8_t pin_data_clock,
	uint8_t pin_storage_clock
){
	/* Allocate memory */
	slave_led* s = malloc(sizeof(slave_led));

	/* Store pin config */
	s->pin_reset = pin_reset;
	s->pin_output_enable = pin_output_enable;
	s->pin_data = pin_data;
	s->pin_data_clock = pin_data_clock;
	s->pin_storage_clock = pin_storage_clock;

	/* Init internal state */
	s->color_red = 0b0;
	s->color_green = 0b0;

	/* Init pins */
	pinMode(s->pin_reset, OUTPUT);
	pinMode(s->pin_output_enable, OUTPUT);
	pinMode(s->pin_data, OUTPUT);
	pinMode(s->pin_data_clock, OUTPUT);
	pinMode(s->pin_storage_clock, OUTPUT);
	digitalWrite(s->pin_reset, HIGH); /* Active low */
	digitalWrite(s->pin_output_enable, LOW); /* Active low */
	digitalWrite(s->pin_data, LOW);
	digitalWrite(s->pin_data_clock, LOW);
	digitalWrite(s->pin_storage_clock, LOW);

	return s;
}

void slave_led_free(slave_led* s){
	/* Free allocated memory */
	free(s);
}

void slave_led_reset(slave_led* s){
	/* Low pulse on reset */
	delay(1); /* 1ms */
	digitalWrite(s->pin_reset, LOW);
	delay(1); /* 1ms */
	digitalWrite(s->pin_reset, HIGH);
	delay(1); /* 1ms */
}

void slave_led_set(slave_led* s, uint8_t led, color_t c){
	/* Update internal state */
	if(c == COLOR_NONE){
		s->color_red &= ~(0b1 << led);
		s->color_green &= ~(0b1 << led);
		return;
	}
	if(c == COLOR_RED || c == COLOR_RED_GREEN){
		s->color_red |= 0b1 << led;
	}
	if(c == COLOR_GREEN || c == COLOR_RED_GREEN){
		s->color_green |= 0b1 << led;
	}
}

void slave_led_show(slave_led* s){
	/* Pull clocks low */
	digitalWrite(s->pin_data_clock, LOW);
	digitalWrite(s->pin_storage_clock, LOW);
	delay(1); /* 1ms */

	/* Write data */
	uint16_t data = (s->color_green << 8) | s->color_red;
	char bit;
	char i;
	for(i=0; i<16; i++){
		/* Get bit */
		bit = data >> 15;
		data = data << 1;

		/* Write bit */
		digitalWrite(s->pin_data, bit);
		delay(1); /* 1ms */

		/* Rising clock stores data */
		digitalWrite(s->pin_data_clock, HIGH);
		delay(1); /* 1ms */
		digitalWrite(s->pin_data_clock, LOW);
		delay(1); /* 1ms */
	}
	digitalWrite(s->pin_data, LOW);

	/* Pulse storage clock */
	delay(1); /* 1ms */
	digitalWrite(s->pin_storage_clock, HIGH);
	delay(1); /* 1ms */
	digitalWrite(s->pin_storage_clock, LOW);
}
