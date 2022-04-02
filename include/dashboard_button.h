#ifndef _H_DASHBOARD_BUTTON
#define _H_DASHBOARD_BUTTON

/* Define LED states */
enum led_state {
	LED_OFF,
	LED_ON,
	LED_BLINK
};
typedef enum led_state led_state_t;

/* Define structure */
typedef struct {
	uint8_t pin_input;
	uint8_t pin_led;
	led_state_t led_state;
} dashboard_button;

/* Initializes a new button object */
dashboard_button* dashboard_button_init(
	uint8_t pin_input,
	uint8_t pin_led
);

/* Free all allocated memory */
void dashboard_button_free(dashboard_button* db);

/* Changes the internal state of the LED */
void dashboard_button_set_led(dashboard_button* db, led_state_t ls);

/* Checks if input changed & Update LED output */
void dashboard_button_update(dashboard_button* db);

#endif
