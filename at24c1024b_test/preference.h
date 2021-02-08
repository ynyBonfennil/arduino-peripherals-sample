#ifndef PREFERENCE_H
#define PREFERENCE_H

/***
   Peripheral Setting
*/
#define USE_AM2320_DHT
//#define USE_DHT11
//#define USE_DUMMY_SENSOR
#define USE_AT24C1024B
//#define USE_DUMMY_EEPROM

/***
   Pin Assignment
*/
//#define PIN_EEPROM_PWR                    D4
#define PIN_SENSOR1_WIRE                  D4
#define PIN_SENSOR2_WIRE                  D5
#define PIN_SENSOR1_PWR                   D6
#define PIN_SENSOR2_PWR                   D7

/***
   Power Saving (should not be defined if you use SERAL_DEBUG)
*/
//#define POWER_SAVING

/**
   Sensing Time
   60 -> every 1 minutes (at 0second)
   20 -> every 20 seconds (0 second, 20 second, 40 second)
   40 -> every 40 seconds (40, 1min20sec)
*/
#define SENSING_TIME                      3600

/***
   State Monitor
*/
#define STATE_MONITOR_TICKER_INTERVAL     10
#define STATE_MONITOR_TIMEOUT             20
#define STATE_MONITOR_ERROR               30

/***
   EEPROM Cache
*/
#define EEPROM_CACHE_LENGTH               480

/***
   EEPROM Bulk Read Length
*/
#define EEPROM_BULK_READ_LENGTH           60

/***
   Sampling Data Length
*/
#define SAMPLING_DATA_LENGTH              20

/***
   Debug Mode Setting
*/
#define SERIAL_DEBUG
#define LED_DEBUG

#endif
