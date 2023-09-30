
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi.h"
#include "http.h"

static const char *TAG = "main";

void app_main(void)
{
	ESP_LOGI(TAG, "ESP32 HTTP Example");

	// Initialise network
	wifi_init_sta();
	http_server_init();

	while(1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}