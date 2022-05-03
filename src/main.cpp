#include <Arduino.h>
#include <FlexCAN_T4.h>
extern "C" {
	#include "slave_led.h"
	#include "dashboard_button.h"
}

/* Pin Definitions */
#define PIN_DEBUG 13
#define PIN_SL_IND_DATA 2
#define PIN_SL_IND_DATA_CLOCK 3
#define PIN_SL_IND_STORAGE_CLOCK 4
#define PIN_SL_AMI_DATA 5
#define PIN_SL_AMI_DATA_CLOCK 6
#define PIN_SL_AMI_STORAGE_CLOCK 7
#define PIN_SL_OUTPUT_ENABLE 8
#define PIN_SL_RESET 9
#define PIN_DB_RTD_IN 17
#define PIN_DB_RTD_LED 15
#define PIN_DB_TS_IN 16
#define PIN_DB_TS_LED 14

/* CAN message IDs */
#define CAN_ID_RTD_PLAY_AUDIO 2

/* Declare vars */
dashboard_button* db_rtd; /* RTD button */
dashboard_button* db_ts; /* TS button */
slave_led* sl_ind; /* Indicator Slave */
slave_led* sl_ami; /* AMI Slave */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;

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

	/* Setup CAN */
	setup_can();
}

/* LOOP */
void loop() {
	/* Handle CAN messages */
	can.events();

	/* Check buttons */
	dashboard_button_update(db_rtd);
	dashboard_button_update(db_ts);
}

