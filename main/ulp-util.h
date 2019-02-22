/*
 * JoBa1 ESP32 ULP blink example
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "ulp-main.h"

void ulp_init( uint32_t wakeup_ms[], size_t wakeups );
void ulp_start();
uint16_t ulp_get( uint32_t ulp_var );
uint16_t ulp_pos( uint32_t ulp_var );
