#ifndef _H_DASHBOARD_BUTTON
#define _H_DASHBOARD_BUTTON

/* Define config */
#define DB_HOLD_TIME 3000 /* Time button needs to be pressed before on_hold is called */

/* Define LED states */
enum led_state {
	LED_OFF,
	LED_ON,
	LED_BLINK,
	LED_STROBE,
	LED_BREATHE
};
typedef enum led_state led_state_t;

/* Define structure */
typedef struct {
	/* Pins */
	uint8_t pin_input;
	uint8_t pin_led;

	/* State */
	led_state_t led_state;
	unsigned long millis_start_press;
	unsigned long millis_start_hold;

	/* Callbacks */
	void (*on_press)(void);
	void (*on_release)(void);
	void (*on_hold)(void);
} dashboard_button;

/* Initializes a new button object */
dashboard_button* dashboard_button_init(
	uint8_t pin_input,
	uint8_t pin_led,
	void (*on_press)(void),
	void (*on_release)(void),
	void (*on_hold)(void)
);

/* Free all allocated memory */
void dashboard_button_free(dashboard_button* db);

/* Changes the internal state of the LED */
void dashboard_button_set_led(dashboard_button* db, led_state_t ls);

/* Checks if input changed & Update LED output */
void dashboard_button_update(dashboard_button* db);

#endif
