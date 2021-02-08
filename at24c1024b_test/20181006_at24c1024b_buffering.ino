#include "eeprom.h"
#include "eeprom_cache.h"
#include "rtc_api.h"

typedef union {
  float value;
  uint8_t bytes[4];
} SAMPLING_DATA;

typedef union {
  uint32_t value;
  uint8_t bytes[4];
} TIMESTAMP;

LowPowerTicker ticker1;
EEPROM eeprom;
EEPROM_CACHE eeprom_cache;
uint32_t counter;

void ticker_handler(void) {
  Serial.println(counter);

  SAMPLING_DATA data1 = {45};
  SAMPLING_DATA data2 = {25};
  SAMPLING_DATA data3 = {44};
  SAMPLING_DATA data4 = {24.8};

  TIMESTAMP stamp = {(uint32_t)rtc_read()};

  uint8_t send_buf[20];
  send_buf[0] = stamp.bytes[0];
  send_buf[1] = stamp.bytes[1];
  send_buf[2] = stamp.bytes[2];
  send_buf[3] = stamp.bytes[3];
  send_buf[4] = data1.bytes[0];
  send_buf[5] = data1.bytes[1];
  send_buf[6] = data1.bytes[2];
  send_buf[7] = data1.bytes[3];
  send_buf[8] = data2.bytes[0];
  send_buf[9] = data2.bytes[1];
  send_buf[10] = data2.bytes[2];
  send_buf[11] = data2.bytes[3];
  send_buf[12] = data3.bytes[0];
  send_buf[13] = data3.bytes[1];
  send_buf[14] = data3.bytes[2];
  send_buf[15] = data3.bytes[3];
  send_buf[16] = data4.bytes[0];
  send_buf[17] = data4.bytes[1];
  send_buf[18] = data4.bytes[2];
  send_buf[19] = data4.bytes[3];

  eeprom.append(send_buf, 20);
  counter += 20;

  uint8_t read_buf[20];

  delay(5);
  if (counter > 7000 * 20) {
    ticker1.detach();
    while (eeprom_cache.read(&eeprom, read_buf, SAMPLING_DATA_LENGTH) != 0) {

      for (int i = 0; i < 4; i++) {
        stamp.bytes[i] = read_buf[i];
      }

      for (int i = 0; i < 4; i++) {
        data1.bytes[i] = read_buf[4 + i];
        data2.bytes[i] = read_buf[8 + i];
        data3.bytes[i] = read_buf[12 + i];
        data4.bytes[i] = read_buf[16 + i];
      }
      Serial.print(stamp.value);
      Serial.print(" ");
      Serial.print(data1.value);
      Serial.print(" ");
      Serial.print(data2.value);
      Serial.print(" ");
      Serial.print(data3.value);
      Serial.print(" ");
      Serial.print(data4.value);
      Serial.println(" ");

      delay(5);
    }
    ticker1.attach_us(ticker_handler, 10000);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("begin");

  rtc_init();
  rtc_write((uint32_t)0);

  eeprom.begin();
  eeprom.clearAll();
  eeprom_cache.clearAll();
  counter = 0;
  ticker1.attach_us(ticker_handler, 10000);

}

void loop() {
  delay(1000);
}
