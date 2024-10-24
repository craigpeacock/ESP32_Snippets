
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "esp_pm.h"

static const char *TAG = "main";

void app_main(void)
{
	esp_err_t ret;
	esp_pm_config_t pm_cfg;

	ESP_LOGI(TAG, "ESP32 Power Management Example");

	ret = esp_pm_get_configuration(&pm_cfg);
	if (ret == ESP_OK) {
		ESP_LOGI(TAG, "MAX CPU Freq %d, MIN CPU Freq %d, Light Sleep Enabled %d", pm_cfg.max_freq_mhz, pm_cfg.min_freq_mhz, pm_cfg.light_sleep_enable);
	}

	pm_cfg.min_freq_mhz = 40;
	pm_cfg.max_freq_mhz = 40;
	//pm_cfg.light_sleep_enable = 1; // Not yet supported according to ESP-IDF Programming Guide:
	// esp_pm_configure() function also has provisions for enabling automatic light sleep mode.
	// However this feature is not fully supported yet, so esp_pm_configure will return an 
	// ESP_ERR_NOT_SUPPORTED if automatic light sleep is requested.

	ret = esp_pm_configure(&pm_cfg);
	if (ret == ESP_OK) {
		ESP_LOGI(TAG, "Successfully configured power management");
	} else {
		ESP_LOGE(TAG, "Error %d configuring power management: %s", ret, esp_err_to_name(ret));
	}

	while(1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}