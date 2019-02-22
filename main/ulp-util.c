/*
 * JoBa1 ESP32 ULP blink example
 */

#include "ulp-util.h"

#include "esp32/ulp.h"

extern const uint8_t _ulp_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t _ulp_end[]   asm("_binary_ulp_main_bin_end");

void ulp_init( uint32_t wakeup_ms[], size_t wakeups ) {
  ESP_ERROR_CHECK( ulp_load_binary(0, _ulp_start, (_ulp_end-_ulp_start)/sizeof(ulp_entry)) );
  if( wakeup_ms ) {
    for( size_t wakeup = 0; wakeup < wakeups; wakeup++ ) {
      ESP_ERROR_CHECK( ulp_set_wakeup_period(wakeup, wakeup_ms[wakeup]) );
    }
  }
}

void ulp_start() {
  ESP_ERROR_CHECK( ulp_run(&ulp_entry-RTC_SLOW_MEM) );
}

uint16_t ulp_get( uint32_t ulp_var ) {
  return (uint16_t)((volatile uint32_t)ulp_var);
}

uint16_t ulp_pos( uint32_t ulp_var ) {
  return (uint16_t)((volatile uint32_t)ulp_var >> 16);
}
