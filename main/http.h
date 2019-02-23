/*
JoBa1 ESP32 post http over wifi
Configure with make menuconfig
*/

#pragma once

#include "esp_system.h"

// wait for wifi connect and post a http request
int post_http( const char *post_data );

// start a wifi connection (async)
esp_err_t start_wifi(void);
