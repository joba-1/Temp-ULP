/*
JoBa1 ESP32 post http over wifi
*/

#include "http.h"

// to wait for connected event
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"

// wifi
#include "esp_wifi.h"
#include "nvs_flash.h"

// http post
#include "esp_http_client.h"
#include <string.h>        // strlen()

// messages
#include "esp_log.h"       // ESP_LOG*()
static const char TAG[] = "http";

esp_err_t http_event_handler( esp_http_client_event_t *evt ) {
  switch(evt->event_id) {
    case HTTP_EVENT_ERROR:
      ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      break;
      ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
      printf("%.*s", evt->data_len, (char*)evt->data);
      break;
    case HTTP_EVENT_ON_DATA:
      break;
      ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (!esp_http_client_is_chunked_response(evt->client)) {
          printf("%.*s", evt->data_len, (char*)evt->data);
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
      break;
  }
  return ESP_OK;
}


static EventGroupHandle_t wifi_event_group;
static const int CONNECTED_BIT = BIT0;


int post_http( const char *post_data ) {
  esp_http_client_config_t config = {
    .url=CONFIG_HTTP_URL,
    .event_handler = http_event_handler,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_method(client, HTTP_METHOD_POST);
  esp_http_client_set_post_field(client, post_data, strlen(post_data));
  esp_http_client_set_header(client, "Content-Type", "application/json");
  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
  esp_http_client_perform(client);
  int status = esp_http_client_get_status_code(client);
  esp_http_client_cleanup(client);
  return status;
}


esp_err_t esp_event_handler( void *ctx, system_event_t *event ) {
  switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      /* This is a workaround as ESP32 WiFi libs don't currently
         auto-reassociate. */
      esp_wifi_connect();
      xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
      break;
    default:
      break;
  }
  return ESP_OK;
}


esp_err_t start_wifi(void) {
  // TODO: better: ESP_LOGE(TAG, "oops: %s", esp_err_to_name(err));
  esp_err_t ret = nvs_flash_init();
  if( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    nvs_flash_init();
  }
  tcpip_adapter_init();
  wifi_event_group = xEventGroupCreate();
  if( (ret = esp_event_loop_init(esp_event_handler, NULL)) != ESP_OK ) return ret;
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  if( (ret = esp_wifi_init(&cfg)) != ESP_OK ) return ret;
  if( (ret = esp_wifi_set_storage(WIFI_STORAGE_RAM)) != ESP_OK ) return ret;
  if( (ret = esp_wifi_set_mode(WIFI_MODE_STA)) != ESP_OK ) return ret;
  wifi_config_t sta_config = {
    .sta = {
      .ssid = CONFIG_WIFI_SSID,
      .password = CONFIG_WIFI_PASSWORD,
    }
  };
  if( (ret = esp_wifi_set_config(WIFI_IF_STA, &sta_config)) != ESP_OK ) return ret;
  return esp_wifi_start();
}
