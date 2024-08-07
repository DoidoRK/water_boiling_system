#include <stdio.h>
#include "nvs_flash.h"
#include "wifi.h"
#include "status_socket.h"
#include "cmd_socket.h"
#include "system_simulation.h"

void app_main() {
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  wifi_init_sta();

  setup_status_socket();
  setup_cmd_socket();
}