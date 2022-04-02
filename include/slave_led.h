#ifndef _H_SLAVE_LED
#define _H_SLAVE_LED

/* Define states */
enum color {
	COLOR_NONE,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_RED_GREEN
};
typedef enum color color_t;

/* Struct definition */
typedef struct {
	uint8_t pin_reset;
	uint8_t pin_output_enable;
	uint8_t pin_data;
	uint8_t pin_data_clock;
	uint8_t pin_storage_clock;

	char color_red;
	char color_green;
} slave_led;

/* Initializes a slave_led and returns a new object */
slave_led* slave_led_init(
	uint8_t pin_reset,
	uint8_t pin_output_enable,
	uint8_t pin_data,
	uint8_t pin_data_clock,
	uint8_t pin_storage_clock
);

/* Frees all allocated memory */
void slave_led_free(slave_led* s);

/* Resets the state and output of a slave_led */
void slave_led_reset(slave_led* s);

/* Sets the internal state of an LED to given color */
void slave_led_set(slave_led* s, uint8_t led, color_t c);

/* Writes the internal state to the physical slave_led */
void slave_led_show(slave_led* s);

#endif
