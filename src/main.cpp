#include <Arduino.h>
#include <FlexCAN_T4.h>
extern "C" {
	#include "config.h"
	#include "slave_led.h"
	#include "dashboard_button.h"
}

/* ========== CONFIG ========== */
/* See file 'include/config.h'! */
/* ============================ */


/* Declare vars */
dashboard_button* db_rtd; /* RTD button */
dashboard_button* db_ts; /* TS button */
slave_led* sl_ind; /* Indicator Slave */
slave_led* sl_ami; /* AMI Slave */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;

unsigned long millis_start_indicator_flash;
char state_indicator_flash;

unsigned long millis_last_ts_ecu_message;
char state_ts = 0b0; /* 0b0 if TS is off, 0b1 if TS is on */
char state_rtd = 0b0; /* 0b0 if RTD is off, 0b1 if RTD is on */

unsigned long millis_bspd_last_message;

/* SDC SIGNALS */
void sdc_input_init(uint8_t pin){
	/* Setup IO */
	pinMode(pin, INPUT);
}
void sdc_input_check(
	uint8_t pin,
	uint8_t ind_led,
	color_t ind_color_fault,
	color_t ind_color_normal
){
	/* Read value */
	uint16_t val = analogRead(pin);
	char state = (val < SDC_THRES); /* Active low */

	/* Change indicator LED */
	slave_led_set(
		sl_ind,
		ind_led,
		state ? ind_color_fault : ind_color_normal
	);
	slave_led_show(sl_ind);
}

/* GPIO SIGNALS */
void gpio_sdc_ts_init(){
	/* Init the GPIO pin for the TS_BTN signal from SDC */
	pinMode(PIN_SDC_TS, INPUT);
	
	/* Init state */
	state_ts = 0b0;
	millis_last_ts_ecu_message = 0UL;
}
void state_ts_update(){
	/* Check if TS is enabled */
	char state_ts_sdc = (analogRead(PIN_SDC_TS) < SDC_THRES) ? 0b1 : 0b0; /* Read signal from SDC */
	char state_ts_ecu = (millis() - millis_last_ts_ecu_message < STATE_TS_ECU_TIMEOUT) ? 0b1 : 0b0; /* Check time since last CAN state update */
	state_ts = state_ts_sdc & state_ts_ecu; /* Both signals need to be active for safety */

	/* Update button LEDs */
	led_state_t l_rtd = state_ts ? (state_rtd ? LED_OFF : LED_BLINK) : LED_OFF;
	led_state_t l_ts = state_ts ? LED_OFF : LED_BLINK;
	dashboard_button_set_led(db_rtd, l_rtd);
	dashboard_button_set_led(db_ts, l_ts);
}
void gpio_tsal_init(){
	/* Init IO */
	pinMode(PIN_TSAL, INPUT);
}
void gpio_tsal_update(){
	/* Read value */
	uint16_t val = analogRead(PIN_TSAL);
	char state = (val > TSAL_THRES);

	/* Update indicator */
	slave_led_set(
		sl_ind,
		LED_TSAL_OFF,
		state ? COLOR_NONE : COLOR_GREEN
	);
	slave_led_show(sl_ind);
}

/* CALLBACKS */
void on_rtd_press(){
	dashboard_button_set_led(db_rtd, LED_ON);
}
void on_rtd_release(){
	dashboard_button_set_led(db_rtd, LED_OFF);
}
void on_rtd_hold(){
	dashboard_button_set_led(db_rtd, LED_STROBE);

	/* Send RTD button pressed */
	if(state_ts && !state_rtd){
		/* Construct CAN message */
		CAN_message_t m;
		m.id = CAN_ID_RTD_BUTTON_PRESSED;
		m.buf[0] = 0x01;
		can.write(m);
	}
}
void on_ts_press(){
	dashboard_button_set_led(db_ts, LED_ON);
}
void on_ts_release(){
	dashboard_button_set_led(db_ts, LED_BREATHE);
}
void on_ts_hold(){
	dashboard_button_set_led(db_ts, LED_STROBE);
}

/* INDICATORS */
void test_indicators(){ /* Enable all LEDs to check if they work */
	uint8_t i, j;
	color_t colors[3] = {COLOR_RED, COLOR_GREEN, COLOR_YELLOW};

	/* Reset registers */
	slave_led_reset(sl_ind);
	slave_led_reset(sl_ami);

	/* Iterate over colors */
	for(j=0; j<3; j++){
		for(i=0; i<8; i++){
			slave_led_set(sl_ind, i, colors[j]);
			slave_led_set(sl_ami, i, colors[j]);
		}
		slave_led_show(sl_ind);
		slave_led_show(sl_ami);
		delay(1000);
	}
}
void indicators_flash_init(){
	/* Set state */
	millis_start_indicator_flash = 0UL;
	state_indicator_flash = 0b0;
}
void indicators_flash_start(){
	/* Set state */
	millis_start_indicator_flash = millis();
	state_indicator_flash = 0b1;
}
void indicators_flash_update(){
	if(!state_indicator_flash) return;

	/* Check if still needs to flash */
	uint16_t delta = millis() - millis_start_indicator_flash;
	if(delta > IND_FLASH_NUM * IND_FLASH_DELAY * 2){
		/* Reset state */
		millis_start_indicator_flash = 0UL;
		state_indicator_flash = 0b0;

		/* Reset output */
		digitalWriteFast(sl_ind->pin_output_enable, LOW);
		return;
	}

	/* Calculate state */
	char s = (delta % (IND_FLASH_DELAY * 2)) < IND_FLASH_DELAY ? 0b1 : 0b0;
	digitalWriteFast(sl_ind->pin_output_enable, s);
}

/* BSPD */
void bspd_init(){
	/* Init IO */
	pinMode(PIN_BSPD_PRESSURE_1, INPUT);
	pinMode(PIN_BSPD_PRESSURE_2, INPUT);
	pinMode(PIN_BSPD_CURRENT, INPUT);

	/* Init state */
	millis_bspd_last_message = 0UL;
}
void bspd_update(){
	/* Check if time to send */
	if(millis() - millis_bspd_last_message > BSPD_CAN_UPDATE_TIME){
		/* Read values */
		uint8_t bspd_p1 = analogRead(PIN_BSPD_PRESSURE_1);
		uint8_t bspd_p2 = analogRead(PIN_BSPD_PRESSURE_2);
		uint8_t bspd_c = analogRead(PIN_BSPD_CURRENT);

		/* Send CAN message */
		CAN_message_t m;
		m.id = CAN_ID_BSPD_STATS;
		m.buf[0] = bspd_p1;
		m.buf[1] = bspd_p2;
		m.buf[2] = bspd_c;
		can.write(m);

		/* Update state */
		millis_bspd_last_message = millis();
	}
}


/* CAN */
void on_can_receive(const CAN_message_t& m){
	switch(m.id){
		case CAN_ID_ECU_TS_STATE:
			/* Update TS state */
			if(m.buf[0] == 0x01){
				millis_last_ts_ecu_message = millis();
			}else{
				millis_last_ts_ecu_message = 0UL;
			}
			break;
		case CAN_ID_RTD_BUTTON_PRESSED:
			/* Update RTD state */
			if(m.buf[2] == 0x01){
				state_rtd = 0b1;
			}
			break;
	}
}

void setup_can(){
	can.begin();
	can.setBaudRate(1000000);
	can.setMaxMB(16);
	can.enableFIFO();
	can.onReceive(on_can_receive);
}

/* SETUP */
void setup() {
	/* Setup IO */
	pinMode(PIN_DEBUG, OUTPUT);
	digitalWrite(PIN_DEBUG, LOW);

	/* Setup buttons */
	db_rtd = dashboard_button_init(
		PIN_DB_RTD_IN,
		PIN_DB_RTD_LED,
		&on_rtd_press,
		&on_rtd_release,
		&on_rtd_hold
	);
	db_ts = dashboard_button_init(
		PIN_DB_TS_IN,
		PIN_DB_TS_LED,
		&on_ts_press,
		&on_ts_release,
		&on_ts_hold
	);

	/* Setup LEDs */
	sl_ind = slave_led_init(
		PIN_SL_RESET,
		PIN_SL_OUTPUT_ENABLE,
		PIN_SL_IND_DATA,
		PIN_SL_IND_DATA_CLOCK,
		PIN_SL_IND_STORAGE_CLOCK
	);
	sl_ami = slave_led_init(
		PIN_SL_RESET,
		PIN_SL_OUTPUT_ENABLE,
		PIN_SL_AMI_DATA,
		PIN_SL_AMI_DATA_CLOCK,
		PIN_SL_AMI_STORAGE_CLOCK
	);
	test_indicators();
	indicators_flash_init();

	/* Setup signals */
	sdc_input_init(PIN_SDC_SDC);
	sdc_input_init(PIN_SDC_ECU);
	sdc_input_init(PIN_SDC_AMS);
	sdc_input_init(PIN_SDC_IMD);
	gpio_sdc_ts_init();
	gpio_tsal_init();
	bspd_init();

	/* Setup CAN */
	setup_can();

}

/* LOOP */
void loop() {
	/* Handle CAN messages */
	can.events();

	/* Check SDC inputs */
	sdc_input_check(PIN_SDC_SDC, LED_SDC_FAULT, COLOR_RED, COLOR_GREEN);
	sdc_input_check(PIN_SDC_ECU, LED_ECU_FAULT, COLOR_RED, COLOR_GREEN);
	sdc_input_check(PIN_SDC_AMS, LED_AMS_FAULT, COLOR_RED, COLOR_NONE);
	sdc_input_check(PIN_SDC_IMD, LED_IMD_FAULT, COLOR_RED, COLOR_NONE);

	/* Check GPIO */
	state_ts_update();  /* Checks if TS is still enabled */
	gpio_tsal_update(); /* Checks TSAL status */

	/* Check buttons */
	dashboard_button_update(db_rtd);
	dashboard_button_update(db_ts);

	/* Update indicators */
	indicators_flash_update();

	/* Update BSPD */
	bspd_update();
}

