
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "main";

#define NVS_NAMESPACE_NAME	"storage"
#define NVS_PARTITION_NAME	"nvs"

#define NVS_KEY_SAMPLE	"MyKey"


void nvs_display_stats(const char *partition_name)
{
	nvs_stats_t nvs_stats;
	nvs_get_stats(partition_name, &nvs_stats);
		ESP_LOGI(TAG, "Count: Used Entries = (%lu), Free Entries = (%lu), Available Entries = (%lu), All Entries = (%lu), Namespaces = (%lu)",
					nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.available_entries, nvs_stats.total_entries, nvs_stats.namespace_count);
}

void nvs_interate_namespace(nvs_handle_t nvs_handle, const char *partition_name, const char *namespace_name)
{
 	nvs_iterator_t it = NULL;
 	esp_err_t res = nvs_entry_find(partition_name, namespace_name, NVS_TYPE_ANY, &it);
	ESP_LOGI(TAG, "Iterating Partition '%s' Namespace '%s':", partition_name, namespace_name);
 	while(res == ESP_OK) {
		esp_err_t err;
		nvs_entry_info_t info;
		nvs_entry_info(it, &info);
		switch (info.type) {
			case NVS_TYPE_U8:
				uint8_t u8_value;
				err = nvs_get_u8(nvs_handle, info.key, &u8_value);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "uint8 '%s' = %d ", info.key, u8_value);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_I8:
				int8_t i8_value;
				err = nvs_get_i8(nvs_handle, info.key, &i8_value);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "int8 '%s' = %d ", info.key, i8_value);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_U16:
				uint16_t u16_value;
				err = nvs_get_u16(nvs_handle, info.key, &u16_value);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "uint16 '%s' = %d ", info.key, u16_value);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_I16:
				int16_t i16_value;
				err = nvs_get_i16(nvs_handle, info.key, &i16_value);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "int16 '%s' = %d ", info.key, i16_value);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_U32:
				uint32_t u32_value;
				err = nvs_get_u32(nvs_handle, info.key, &u32_value);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "uint32 '%s' = %d ", info.key, u32_value);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_I32:
				int32_t i32_value;
				err = nvs_get_i32(nvs_handle, info.key, &i32_value);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "int32 '%s' = %d ", info.key, i32_value);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_U64:
				uint64_t u64_value;
				err = nvs_get_u64(nvs_handle, info.key, &u64_value);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "uint64 '%s' = %d ", info.key, u64_value);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_I64:
				int64_t i64_value;
				err = nvs_get_i64(nvs_handle, info.key, &i64_value);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "int64 '%s' = %d ", info.key, i64_value);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_STR:
				char buffer[256];
				size_t len = sizeof(buffer);
				err = nvs_get_str(nvs_handle, info.key, (char *)&buffer, &len);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "String '%s' = '%s' (Length %d bytes)", info.key, buffer, len);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			case NVS_TYPE_BLOB:
				err = nvs_get_blob(nvs_handle, info.key, (void *)&buffer, &len);
				if (err == ESP_OK) {
					ESP_LOGI(TAG, "Blob '%s' = '%s' (Length %d bytes)", info.key, buffer, len);
				} else {
					ESP_LOGE(TAG, "Error (%s) reading key '%s'", esp_err_to_name(err), info.key);
				}
				break;

			default:
				ESP_LOGE(TAG, "Unknown type (%d) for key '%s'", info.type, info.key);
				break;
		}

		res = nvs_entry_next(&it);
	}
	nvs_release_iterator(it);
}

void app_main(void)
{
	ESP_LOGI(TAG, "ESP32 NVS Example");

	esp_err_t err;
	nvs_handle_t nvs_handle;

	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_LOGW(TAG, "%s: Erasing flash", esp_err_to_name(err));
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

	nvs_display_stats(NVS_PARTITION_NAME);

	err = nvs_open(NVS_NAMESPACE_NAME, NVS_READWRITE, &nvs_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) opening NVS handle", esp_err_to_name(err));
		return;
	}

	nvs_interate_namespace(nvs_handle, NVS_PARTITION_NAME, NVS_NAMESPACE_NAME);

	uint16_t value = 54;

	err = nvs_set_u16(nvs_handle, NVS_KEY_SAMPLE, value);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) setting %s", esp_err_to_name(err), NVS_KEY_SAMPLE);
	}
	
	err = nvs_get_u16(nvs_handle, NVS_KEY_SAMPLE, &value);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) reading %s", esp_err_to_name(err), NVS_KEY_SAMPLE);
	}

	nvs_close(nvs_handle);

	while(1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}