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

unsigned long millis_start_sdc_ts_btn;
char state_sdc_ts_btn;

/* SDC SIGNALS */
void sdc_input_init(uint8_t pin){
	/* Setup IO */
	pinMode(pin, INPUT);
}
void sdc_input_check(
	uint8_t pin,
	uint8_t ind_led,
	color_t ind_color
){
	/* Read value */
	uint16_t val = analogRead(pin);
	char state = (val < SDC_THRES);

	/* Change indicator LED */
	slave_led_set(
		sl_ind,
		ind_led,
		state ? ind_color : COLOR_NONE
	);
	slave_led_show(sl_ind);
}

/* GPIO SIGNALS */
void gpio_sdc_ts_btn_init(){
	/* Init the GPIO pin for the TS_BTN signal to SDC */
	pinMode(PIN_SDC_TS_BTN, OUTPUT);
	digitalWriteFast(PIN_SDC_TS_BTN, LOW);
	
	/* Init state */
	state_sdc_ts_btn = 0b0;
	millis_start_sdc_ts_btn = 0UL;
}
void gpio_sdc_ts_btn_update(){
	/* Set correct output for TS_BTN signal */
	if(state_sdc_ts_btn){
		/* Check if needs to stop pulse */
		if(millis() - millis_start_sdc_ts_btn > SDC_TS_BTN_PULSE_MILLIS){
			/* Disable signal */
			digitalWriteFast(PIN_SDC_TS_BTN, LOW);

			/* Reset state */
			state_sdc_ts_btn = 0;
			millis_start_sdc_ts_btn = 0UL;
		}
	}
}
void gpio_sdc_ts_btn_pulse(){
	/* Start pulse on TS_BTN signal to enable TS */
	digitalWriteFast(PIN_SDC_TS_BTN, HIGH);

	/* Set state for timeout */
	state_sdc_ts_btn = 0b1;
	millis_start_sdc_ts_btn = millis();
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
}
void on_ts_press(){
	dashboard_button_set_led(db_ts, LED_ON);
}
void on_ts_release(){
	dashboard_button_set_led(db_ts, LED_BREATHE);
}
void on_ts_hold(){
	dashboard_button_set_led(db_ts, LED_STROBE);
	gpio_sdc_ts_btn_pulse(); /* Activate TS */
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


/* CAN */
void on_can_receive(const CAN_message_t &msg){

}

void setup_can(){
	can.begin();
	can.setBaudRate(1000000);
	can.setMaxMB(16);
	can.enableFIFO();
	can.enableFIFOInterrupt();
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

	/* Setup signals */
	sdc_input_init(PIN_SDC_SDC);
	sdc_input_init(PIN_SDC_ECU);
	sdc_input_init(PIN_SDC_AMS);
	sdc_input_init(PIN_SDC_IMD);
	gpio_sdc_ts_btn_init();

	/* Setup CAN */
	setup_can();
}

/* LOOP */
void loop() {
	/* Handle CAN messages */
	can.events();

	/* Check SDC inputs */
	sdc_input_check(PIN_SDC_SDC, LED_SDC_FAULT, COLOR_RED);
	sdc_input_check(PIN_SDC_ECU, LED_ECU_FAULT, COLOR_RED);
	sdc_input_check(PIN_SDC_AMS, LED_AMS_FAULT, COLOR_RED);
	sdc_input_check(PIN_SDC_IMD, LED_IMD_FAULT, COLOR_RED);

	/* Check GPIO */
	gpio_sdc_ts_btn_update();

	/* Check buttons */
	dashboard_button_update(db_rtd);
	dashboard_button_update(db_ts);
}

