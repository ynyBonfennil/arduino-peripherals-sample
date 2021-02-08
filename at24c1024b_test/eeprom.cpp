/*
 * Written by Tomoaki Osada
 */

#include "eeprom.h"

#ifdef USE_AT24C1024B

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Public
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
   Prepare for using EEPROM
*/
void EEPROM::begin() {

#ifdef PIN_EEPROM_PWR
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
#endif

  Wire.begin();
}

/**
   Get the address of the next read

   @return   read address

   @note     It does not roll over if it reaches at the end of serial eeprom.
             Hence 0x20000, 0x40000, ... are equal to 0x0.

   @note     If write_address takes over read_address (which means buffering data is more than 1024kbit)
             then read_address moves to ahead of write_address.
*/
uint32_t EEPROM::get_read_address() {

  uint32_t read_address = 0;
  uint32_t write_address = 0;

  if (read_address_cache != 0) {
    read_address = read_address_cache;
  }
  else {
    ADDRESS_POINTER addr_ptr = {0};

    uint8_t device_address = DEV_ADDR;
    Wire.beginTransmission(device_address);
    Wire.write((uint8_t)0);
    Wire.write((uint8_t)0);
    delay(5);
    Wire.endTransmission();
    Wire.requestFrom(device_address, (uint8_t)4);
    fetch_available(addr_ptr.bytes);

    read_address = addr_ptr.value;
  }

  write_address = get_write_address();
  if (0x1F400 < write_address && read_address < write_address - 0x1F400) {
    read_address = write_address - 0x1F400;
  }
  Serial.begin(9600);
  Serial.print("read_address: ");
  Serial.println(read_address);
  Serial.print("write_address: ");
  Serial.println(write_address);

  return read_address;
}

/**
   Get the address where next data will be written

   @return   write address

   @note     It does not roll over if it reaches at the end of serial eeprom.
             Hence 0x20000, 0x40000, ... are equal to 0x0.
*/
uint32_t EEPROM::get_write_address() {

  if (write_address_cache != 0) {
    return write_address_cache;
  }

  ADDRESS_POINTER addr_ptr = {0};

  uint8_t device_address = DEV_ADDR;
  Wire.beginTransmission(device_address);
  Wire.write((uint8_t)0);
  Wire.write((uint8_t)4);
  delay(5);
  Wire.endTransmission();
  Wire.requestFrom(device_address, (uint8_t)4);
  fetch_available(addr_ptr.bytes);

  return addr_ptr.value;
}

/**
   Write data to serial eeprom (which acts as a FIFO)

   @param   pbuf
              array that contains data to be written

   @param   len
              length of the data to be written
*/
void EEPROM::append(uint8_t *pbuf, uint16_t len) {
  uint32_t write_address = get_write_address();

  if (write_address / 0x100 != (write_address + len - 1) / 0x100) {
    uint16_t first_half = 0x100 - (write_address & 0xff);
    uint16_t second_half = len - first_half;
    this->append(pbuf, first_half);
    this->append(pbuf + first_half, second_half);
  }
  else {

    if (write_address % 0x1F400 == 0) {
      write_address += INDENT;
    }
    
    uint8_t device_address = (uint8_t)(DEV_ADDR + (write_address / 0xFA00) % 2);
    Wire.beginTransmission(device_address);
    Wire.write((uint8_t)((write_address % 0xFA00) >> 8));
    Wire.write((uint8_t)(write_address % 0xFA00));
    delay(5);
    Wire.write(pbuf, len);
    Wire.endTransmission();

    update_write_address(write_address + len);
  }
}

/**
   Read data from serial eeprom (which acts as a FIFO)

   @param    pbuf
               array into which data will be stored

   @param    len
               length of the data to be read

   @return   length of the actual read data
*/
uint16_t EEPROM::read(uint8_t *pbuf, uint16_t len) {

  /* 読み込み量が 60Bytes を超える場合（何故か読み込みに失敗するため分割 read する）*/
  if (len > EEPROM_BULK_READ_LENGTH) {
    uint16_t first_half = 60;
    uint16_t second_half = len - 60;
    uint16_t counter = 0;
    counter += this->read(pbuf, first_half);
    counter += this->read(pbuf + first_half, second_half);
    return counter;
  }

  // Optimize this
  uint32_t read_address = get_read_address();
  uint32_t write_address = get_write_address();
  uint32_t remaining = write_address - read_address;

  if (remaining <= 0) {
    return remaining;
  }

  /* 読み込み残量が要求量以下（ただし0より多い）の場合 */
  if (remaining <= len) {
    if (read_address / 0x100 != (read_address + remaining - 1) / 0x100) {
      uint16_t first_half = 0x100 - (read_address & 0xff);
      uint16_t second_half = remaining - first_half;
      uint16_t counter = 0;
      counter += this->read(pbuf, first_half);
      counter += this->read(pbuf + first_half, second_half);
      return counter;
    }
    else {

      if (read_address % 0x1F400 == 0) {
        read_address += INDENT;
      }
      
      uint8_t counter = 0;
      uint8_t device_address = (uint8_t)(DEV_ADDR + (read_address / 0xFA00) % 2);
      Wire.beginTransmission(device_address);
      Wire.write((uint8_t)((read_address % 0xFA00) >> 8));
      Wire.write((uint8_t)(read_address % 0xFA00));
      delay(5);
      Wire.endTransmission();
      Wire.requestFrom(device_address, (uint8_t)remaining);
      counter += fetch_available(pbuf);
      read_address += counter;
      update_read_address(read_address);
      return counter;
    }
  }

  /* 読み込み残量が要求量より多い場合 */
  else {
    if (read_address / 0x100 != (read_address + len - 1) / 0x100) {
      uint16_t first_half = 0x100 - (read_address & 0xff);
      uint16_t second_half = len - first_half;
      uint16_t counter = 0;
      counter += this->read(pbuf, first_half);
      counter += this->read(pbuf + first_half, second_half);
      return counter;
    }
    else {

      if (read_address % 0x1F400 == 0) {
        read_address += INDENT;
      }
      
      uint8_t counter = 0;
      uint8_t device_address = (uint8_t)(DEV_ADDR + (read_address / 0xFA00) % 2);
      Wire.beginTransmission(device_address);
      Wire.write((uint8_t)((read_address % 0xFA00) >> 8));
      Wire.write((uint8_t)(read_address % 0xFA00));
      delay(5);
      Wire.endTransmission();
      Wire.requestFrom((uint8_t)device_address, (uint8_t)len);
      counter += fetch_available(pbuf);
      read_address += counter;
      update_read_address(read_address);
      return counter;
    }
  }

  return 0;
}

/**
   Clear serial eeprom (which acts as a FIFO)
*/
void EEPROM::clearAll() {
  update_read_address(INDENT);
  update_write_address(INDENT);
  read_address_cache = 0;
  write_address_cache = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Private
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
   Get data from I2C rx buffer
*/
uint8_t EEPROM::fetch_available(uint8_t *pbuf) {
  uint8_t counter = 0;
  while (Wire.available() > 0) {
    uint8_t c = Wire.read();
    pbuf[counter] = c;
    counter += 1;
  }
  return counter;
}

/**
   Update the value of read_address (both cache and serial eeprom)
*/
void EEPROM::update_read_address(uint32_t address) {

  read_address_cache = address;

  ADDRESS_POINTER addr_ptr = {address};

  uint8_t device_address = DEV_ADDR;
  Wire.beginTransmission(device_address);
  Wire.write((uint8_t)0);
  Wire.write((uint8_t)0);
  delay(5);
  Wire.write(addr_ptr.bytes, 4);
  Wire.endTransmission();
}

/**
   Update the value of write_address (both cache and serial eeprom)
*/
void EEPROM::update_write_address(uint32_t address) {

  write_address_cache = address;

  ADDRESS_POINTER addr_ptr = {address};

  uint8_t device_address = DEV_ADDR;
  Wire.beginTransmission(device_address);
  Wire.write((uint8_t)0);
  Wire.write((uint8_t)4);
  delay(5);
  Wire.write(addr_ptr.bytes, 4);
  Wire.endTransmission();
}

#endif
