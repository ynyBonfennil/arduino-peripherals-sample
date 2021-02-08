/**
 * Written by Tomoaki Osada
*/

#include "eeprom_cache.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Public
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
   Getter of private variable "cache_empty"

   @return  true if the cache memory is empty,
            false if it is not.
*/
bool EEPROM_CACHE::is_cache_empty(void) {
  return cache_empty;
}

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
uint16_t EEPROM_CACHE::read(EEPROM *arg_eeprom, uint8_t *pbuf, uint16_t len) {
  uint16_t cache_remaining = cache_write_addr - cache_read_addr;
  uint32_t uncached_len = arg_eeprom->get_write_address() - arg_eeprom->get_read_address();

#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.print("Cache remaining: ");
  Serial.println(cache_remaining);
  Serial.print("Uncached: ");
  Serial.println(uncached_len);
#endif

  if (cache_remaining == 0 && uncached_len > 0) {
    cache_write_addr = 0;
    cache_read_addr = 0;
    arg_eeprom->begin();
    cache_write_addr = arg_eeprom->read(cache, EEPROM_CACHE_LENGTH);
    cache_remaining = cache_write_addr - cache_read_addr;
  }

  if (cache_remaining != 0) {
    if (cache_remaining < len) {
      for (int i = 0; i < cache_remaining; i++) {
        pbuf[i] = cache[cache_read_addr + i];
      }
      cache_read_addr += cache_remaining;
      return cache_remaining;
    }
    else {
      for (int i = 0; i < len; i++) {
        pbuf[i] = cache[cache_read_addr + i];
      }
      cache_read_addr += len;
      return len;
    }
  }

  return 0;
}

/**
   Clear cache memory.
*/
void EEPROM_CACHE::clearAll(void) {
  cache_write_addr = 0;
  cache_read_addr = 0;
  cache_empty = true;
}
