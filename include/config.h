#ifndef _H_CONFIG
#define _H_CONFIG

/* PINS */
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
#define PIN_SDC_SDC 18
#define PIN_SDC_ECU 19
#define PIN_SDC_AMS 20
#define PIN_SDC_IMD 21
#define PIN_GPIO_1 0
#define PIN_GPIO_2 1
#define PIN_GPIO_3 10
#define PIN_GPIO_4 11
#define PIN_SDC_TS_BTN PIN_GPIO_1 /* Alias */
#define PIN_BSPD_PRESSURE_1 PIN_GPIO_2 /* Alias */
#define PIN_BSPD_PRESSURE_2 PIN_GPIO_3 /* Alias */
#define PIN_BSPD_CURRENT PIN_GPIO_4 /* Alias */

/* CAN IDs */
#define CAN_ID_RTD_PLAY_AUDIO 2

/* INDICATORS */
#define LED_SDC_FAULT 0
#define LED_ECU_FAULT 1
#define LED_AMS_FAULT 2
#define LED_IMD_FAULT 3

/* MISC */
#define SDC_THRES 930 /* Threshold below which SDC input signals are considered ACTIVE (3.3V = 1024) */

#endif
