/**
 * Written by Tomoaki Osada
*/

#ifndef EEPROM_CACHE_H
#define EEPROM_CACHE_H

#include <Arduino.h>
#include <Wire.h>
#include "eeprom.h"
#include "preference.h"

/**
   Simple FIFO buffer that acts as a serial eeprom read cache
*/
class EEPROM_CACHE {

  public:

    /**
       Getter of private variable "cache_empty"

       @return  true if the cache memory is empty,
                false if it is not.
    */
    bool is_cache_empty(void);

    /**
       Read data from eeprom through eeprom cache.
       If cache has some data, then it returns data from cache memory.
       If cache is empty, it fetches data from eeprom, fill the cache memory,
       and then return the data.

       @param   eeprom
                  The eeprom that uses this cache memory

       @param   pbuf
                  buffer to store read data

       @param   len
                  length of read data
    */
    uint16_t read(EEPROM *eeprom, uint8_t *pbuf, uint16_t len);

    /**
       Clear cache memory.
    */
    void clearAll(void);

  private:

    /**
       The actual cache memory
    */
    uint8_t cache[EEPROM_CACHE_LENGTH ];

    /**
       Index into where the next data will be stored
    */
    uint16_t cache_write_addr ;

    /**
       Index from where the next data will be read
    */
    uint16_t cache_read_addr ;

    /**
       The flag that signs whether the cache is empty or not
    */
    bool cache_empty ;

};

#endif
