/**
 * Written by Tomoaki Osada
*/

#ifndef EEPROM_H
#define EEPROM_H

#include <Arduino.h>
#include <Wire.h>
#include "preference.h"

/**
   Class to treat serail eeprom as a FIFO buffer.
   It is designed to be used for AT24C1024B
*/
class EEPROM {

  public:

    /**
       Prepare for using EEPROM
    */
    void begin(void);

    /**
       Get the address of the next read

       @return   read address

       @note     It does not roll over if it reaches at the end of serial eeprom.
                 Hence 0x20000, 0x40000, ... are equal to 0x0.
    */
    uint32_t get_read_address(void);

    /**
       Get the address where next data will be written

       @return   write address

       @note     It does not roll over if it reaches at the end of serial eeprom.
                 Hence 0x20000, 0x40000, ... are equal to 0x0.
    */
    uint32_t get_write_address(void);

    /**
       Write data to serial eeprom (which acts as a FIFO)

       @param   pbuf
                  array that contains data to be written

       @param   len
                  length of the data to be written
    */
    void append(uint8_t *pbuf, uint16_t len);

    /**
       Read data from serial eeprom (which acts as a FIFO)

       @param    pbuf
                   array into which data will be stored

       @param    len
                   length of the data to be read

       @return   length of the actual read data
    */
    uint16_t read(uint8_t *pbuf, uint16_t len);

    /**
       Clear serial eeprom (which acts as a FIFO)
    */
    void clearAll(void);

  private:

    /**
       I2C device address
    */
    static const uint8_t DEV_ADDR = 0b1010000;

    /**
       Length of reserved area for storing read_address and write_address value.

       Since both read_address and write_address are 4 bytes, this value must be
       over 8 bytes.
    */
    static const uint32_t INDENT = 20;

    /**
       Cache for read_address.

       Since getting the value of read_address from serial eeprom takes time,
       It always keeps the value at this cache.
    */
    uint32_t read_address_cache;

    /**
       Cache for write_address.

       Since getting the value of write_address from serial eeprom takes time,
       It always keeps the value at this cache.
    */
    uint32_t write_address_cache;

    /**
       Get data from I2C rx buffer
    */
    uint8_t fetch_available(uint8_t *pbuf);

    /**
       Update the value of read_address (both cache and serial eeprom)
    */
    void update_read_address(uint32_t address);

    /**
       Update the value of write_address (both cache and serial eeprom)
    */
    void update_write_address(uint32_t address);

    typedef union {
      uint32_t value;
      uint8_t bytes[4];
    } ADDRESS_POINTER;

};

#endif
