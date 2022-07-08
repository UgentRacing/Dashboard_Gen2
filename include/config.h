#ifndef _H_CONFIG
#define _H_CONFIG

/* PINS */
#define PIN_DEBUG                13
#define PIN_SL_IND_DATA          2
#define PIN_SL_IND_DATA_CLOCK    3
#define PIN_SL_IND_STORAGE_CLOCK 4
#define PIN_SL_AMI_DATA          5
#define PIN_SL_AMI_DATA_CLOCK    6
#define PIN_SL_AMI_STORAGE_CLOCK 7
#define PIN_SL_OUTPUT_ENABLE     8
#define PIN_SL_RESET             9
#define PIN_DB_RTD_IN            17
#define PIN_DB_RTD_LED           15
#define PIN_DB_TS_IN             16
#define PIN_DB_TS_LED            14
#define PIN_GPIO_1               0
#define PIN_GPIO_2               1
#define PIN_GPIO_3               10
#define PIN_GPIO_4               11
#define PIN_GPIO_5               12
#define PIN_SDC_TS               PIN_GPIO_1 /* Alias */
#define PIN_SDC_SDC              PIN_GPIO_2 /* Alias */
#define PIN_SDC_ECU              PIN_GPIO_3 /* Alias */
#define PIN_SDC_AMS              PIN_GPIO_4 /* Alias */
#define PIN_SDC_IMD              PIN_GPIO_5 /* Alias */
#define PIN_BSPD_PRESSURE_1      19         /* Label: SDC BSPD */
#define PIN_BSPD_PRESSURE_2      20         /* Label: SDC AMS */
#define PIN_BSPD_CURRENT         21         /* Label: SDC IMD */
#define PIN_TSAL                 18         /* Label: SDC AS */

/* CAN IDs */
#define CAN_ID_RTD_PLAY_AUDIO     0x03
#define CAN_ID_RTD_BUTTON_PRESSED 0x03
#define CAN_ID_ECU_TS_STATE       0x01
#define CAN_ID_BSPD_STATS         0x06
#define CAN_ID_HEARTBEAT          0x07

/* INDICATORS */
#define LED_SDC_FAULT    0
#define LED_ECU_FAULT    1
#define LED_AMS_FAULT    2
#define LED_IMD_FAULT    3
#define LED_TSAL_OFF     4
#define LED_CAN_INACTIVE 5

/* MISC */
#define SDC_THRES               930  /* Threshold below which SDC input signals are considered ACTIVE (3.3V = 1024) */
#define SDC_TS_BTN_PULSE_MILLIS 1000 /* Time TS_BTN signal needs to be high after button hold to enable TS */
#define TSAL_THRES              400  /* Threshold above which TSAL input signal is considered ACTIVE (3.3V = 1024) */
#define IND_FLASH_NUM           4    /* Number of times the indicator LEDs flash when warning is triggered */
#define IND_FLASH_DELAY         50   /* Delay between flashes of the indicator LEDs flash when warning is triggered */
#define STATE_TS_ECU_TIMEOUT    500  /* Millis after an ECU can TS on message that is will go back to TS off */
#define BSPD_CAN_UPDATE_TIME    100  /* Millis between sending the BSPD stats over CAN */
#define CAN_ACTIVE_TIMEOUT      100  /* Millis after last CAN message when it will be seen as inactive */
#define CAN_HEARTBEAT_PERIOD    50   /* Millis between CAN heartbeat messages */
#define CAN_HEARTBEAT_PAYLOAD   1    /* Heartbeat ID of dashboard */

#endif
