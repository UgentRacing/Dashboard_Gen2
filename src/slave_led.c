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
	s->dirty = 0b1;

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
	/* Reset state */
	s->color_green = 0b0;
	s->color_red = 0b0;
	s->dirty = 0b1;

	/* Low pulse on reset */
	digitalWrite(s->pin_reset, LOW);
	delayNanoseconds(100);
	digitalWrite(s->pin_reset, HIGH);
	delayNanoseconds(100);
}

void slave_led_set(slave_led* s, uint8_t led, color_t c){
	/* Save old state */
	char old_green = s->color_green;
	char old_red = s->color_red;

	/* Update internal state */
	if(c == COLOR_NONE){
		s->color_red &= ~(0b1 << led);
		s->color_green &= ~(0b1 << led);
	}else{
		if(c == COLOR_RED || c == COLOR_YELLOW){
			s->color_red |= 0b1 << led;
		}
		if(c == COLOR_GREEN || c == COLOR_YELLOW){
			s->color_green |= 0b1 << led;
		}
	}

	/* Check if changed */
	if(old_green != s->color_green || old_red != s->color_red){
		s->dirty = 0b1;
	}
}

void slave_led_show(slave_led* s){
	/* Only update if dirty bit set */
	if(s->dirty == 0b0) return;

	/* Pull clocks low */
	digitalWriteFast(s->pin_data_clock, LOW);
	digitalWriteFast(s->pin_storage_clock, LOW);
	delayNanoseconds(25); /* Wait time pulse low - Setup time */

	/* Setup bitstream */
	uint16_t data = 0b0;
	char data_green = s->color_green;
	char data_red = s->color_red;
	char i;
	for(i=0; i<8; i++){
		data = data << 1;
		data |= data_green & 0b1;
		data = data << 1;
		data |= data_red & 0b1;

		data_green = data_green >> 1;
		data_red = data_red >> 1;
	}

	/* Write data */
	char bit;
	for(i=0; i<16; i++){
		/* Get bit */
		bit = data >> 15;
		data = data << 1;

		/* Write bit */
		digitalWriteFast(s->pin_data, bit);
		delayNanoseconds(25); /* Setup time */

		/* Rising clock stores data */
		digitalWriteFast(s->pin_data_clock, HIGH);
		delayNanoseconds(50); /* Wait time pulse high */
		digitalWriteFast(s->pin_data_clock, LOW);
		delayNanoseconds(25); /* Wait time pulse low - Setup time */
	}
	digitalWriteFast(s->pin_data, LOW);
	delayNanoseconds(25); /* Setup time */

	/* Pulse storage clock */
	digitalWriteFast(s->pin_storage_clock, HIGH);
	delayNanoseconds(50); /* Wait time pulse high */
	digitalWriteFast(s->pin_storage_clock, LOW);
	delayNanoseconds(50); /* Wait time pulse low */

	/* Reset dirty bit */
	s->dirty = 0b0;
}

