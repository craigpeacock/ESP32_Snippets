
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "main";

void app_main(void)
{
	ESP_LOGI(TAG, "ESP32 Power Management Example");

	while(1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}