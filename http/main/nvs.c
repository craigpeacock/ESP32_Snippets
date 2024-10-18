
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "nvs";

void nvs_start(void)
{
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	if (ret == ESP_OK)
		ESP_LOGI(TAG, "NVS Storage Initialised");

}

void nvs_stop(void)
{
	esp_err_t ret = nvs_flash_deinit();
	if (ret == ESP_OK)
		ESP_LOGI(TAG, "NVS Storage Deinitialised");

	if (ret == ESP_ERR_NVS_NOT_INITIALIZED)
		ESP_LOGI(TAG, "NVS Storage was not initialised");
}