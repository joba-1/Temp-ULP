/*
JoBa1 ESP32 ULP comm example

Load and start an ULP programm, then read data stream from ULP
If there is no more data, go to sleep and wake up on ULP trigger
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // for task and timing
#include "cJSON.h"
#include "esp_sleep.h"     // esp_sleep_enable_ulp_wakeup(), esp_deep_sleep_start()
#include "esp_log.h"       // ESP_LOG*()
static const char TAG[] = "Temp-ULP";


#include "ulp-util.h"      // my ulp_init(), ulp_start(), ulp_get() and .globals
#include "http.h"          // my start_wifi() and post_http()

uint32_t *buffer = &ulp_buffer;
uint16_t buffer_start_ulp; // buffer start in ulp address space
uint16_t buffer_end_ulp;   // buffer end in ulp address space


// Get one 16 bit value from the ringbuffer that is filled by the ULP,
// and increment the tail pointer to make room for more ULP input
bool get( uint16_t *item ) {
  uint16_t tail = ulp_get(ulp_tail);
  uint16_t head = ulp_get(ulp_head);
  if( tail == head ) return false; // empty buffer!
  *item = ulp_get(buffer[tail-buffer_start_ulp]);
  if( ++tail == buffer_end_ulp ) tail = buffer_start_ulp;
  ulp_tail = tail;
  return true; // item contains valid value
}


void read_ulp() {
  uint16_t item;

  while( (buffer_start_ulp = ulp_get(ulp_offset)) == 0 )
    vTaskDelay(1 / portTICK_PERIOD_MS); // wait until ulp is initialized
  buffer_end_ulp = buffer_start_ulp + (&ulp_buffer_end - &ulp_buffer);

  cJSON *root, *array;
  root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "version", 1);
  cJSON_AddStringToObject(root, "name", "Temp-ULP");
  array = cJSON_CreateArray();
  cJSON_AddItemToObject(root, "raw", array);
  while( get(&item) ) {
    ESP_LOGI(TAG, "raw: %5u", item);
    cJSON_AddItemToArray(array, cJSON_CreateNumber(item));
  }

  post_http(cJSON_Print(root));

  ESP_LOGI(TAG, "Going to sleep now\n");
  esp_deep_sleep_start();
}


void app_main()
{
  ESP_LOGI(TAG, "Hello Temperature ULP!");

  start_wifi();

  if( esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_ULP ) {
    uint32_t wakeups[] = { CONFIG_ULP_DELAY*1000 };
    ulp_init(wakeups, sizeof(wakeups)/sizeof(*wakeups));
    ulp_start();
    ESP_LOGI(TAG, "Started ULP");
  }

  ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup() );

  xTaskCreate(read_ulp, "read_ulp", 4*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
}
