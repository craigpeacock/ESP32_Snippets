
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs.h"
#include "wifi.h"
#include "http.h"

static const char *TAG = "main";

void app_main(void)
{
	ESP_LOGI(TAG, "ESP32 HTTP Example");

	do {

		// Initialise network
		nvs_start();
		wifi_start();
		http_server_start();

		// Stay up for 10 seconds
		vTaskDelay(10000 / portTICK_PERIOD_MS);

		// Now stop
		http_server_stop();
		wifi_stop();
		nvs_stop();
	
		// Stay down for 5 seconds
		vTaskDelay(5000 / portTICK_PERIOD_MS);

	} while(1);
}