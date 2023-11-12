/* 
 * SPIFFS (SPI Flash File System )
 * SPIFFS does not support directories, it produces a flat structure.
 *
 * Ensure the following config options are set:
 * CONFIG_PARTITION_TABLE_CUSTOM=y
 * CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions.csv"
 *
 * If you want to include files in the SPIFFS at build time, add 
 * spiffs_create_partition_image(storage <folder> FLASH_IN_PROJECT)
 * to your CMakeLists.txt where <folder> is the name of your folder
 * containing the SPIFFS
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "driver/gpio.h"
#include "main.h"
#include "wifi.h"
#include "http.h"
#include "uart.h"

static const char *TAG = "main";

static FILE* log_file;

void spiffs_mount(void)
{
	ESP_LOGI(TAG, "Initializing SPIFFS");

	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/data",
		.partition_label = NULL,
		.max_files = 5,
		.format_if_mount_failed = true
	};

	// Use settings defined above to initialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is an all-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		}
		return;
	}
}

void spiffs_check(const char *partition_label)
{
	esp_err_t ret = esp_spiffs_check(partition_label);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "SPIFFS Check Failed (%s).", esp_err_to_name(ret));
		return;
	} else {
		ESP_LOGI(TAG, "SPIFFS Check Successful.");
	}
}

void spiffs_info(const char *partition_label)
{
	size_t total = 0;
	size_t used = 0;

	esp_err_t ret = esp_spiffs_info(partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
		esp_spiffs_format(partition_label);
		return;
	} else {
		ESP_LOGI(TAG, "Partition size: total: %d bytes, used: %d bytes", total, used);
	}
}

void file_list_dir(char *path)
{
	DIR* dir = opendir(path);
	
	if (dir == NULL) {
		ESP_LOGE(TAG, "Cannot open directory");
		return;
	}

	ESP_LOGI(TAG, "Listing files:");

	while (true) {
		struct dirent* de = readdir(dir);
		
		if (de != NULL) {
			ESP_LOGI(TAG, "%s", de->d_name);
		} else {
			break;
		}
	}

	closedir(dir);
}

size_t remove_ansi_escape_codes(char *input, char *output, size_t len)
{
	char *ptr_out = output;
	char *ptr_in = input;

	while (ptr_in <= (input + len - 1)) {
		if (*ptr_in == '\033') {
			// Search for 'm';
			ptr_in = strchr(ptr_in, 'm') + 1;
		} else {
			*ptr_out++ = *ptr_in++;
		}
	}
	return ((ptr_out -1) - output);
}

void process_received_data(char * buf, size_t len)
{
	char buffer[512];
	size_t newlen;
	newlen = remove_ansi_escape_codes(buf, buffer, len);
	ESP_LOGW(TAG, "Recv %.*s", newlen, buffer);
	fwrite(buffer, 1, newlen, log_file);
	fflush(log_file);
}

void app_main(void)
{
	ESP_LOGI(TAG, "ESP32 SPIFFS Example");

	spiffs_mount();

 	// When the chip experiences a power loss during a file system operation it could 
 	// result in SPIFFS corruption. However the file system still might be recovered 
 	// via esp_spiffs_check function.
	//spiffs_check(NULL);

	spiffs_info(NULL);

	file_list_dir("/data");

	gpio_reset_pin(GPIO_HEARTBEAT_LED);
	gpio_set_level(GPIO_HEARTBEAT_LED, 1);
	gpio_set_direction(GPIO_HEARTBEAT_LED, GPIO_MODE_OUTPUT);

	// Initialise network
	wifi_init_sta();
	http_server_init();

	// Create log file for writting
	if ((log_file = fopen("/data/log.txt", "a")) == NULL) {
		ESP_LOGE(TAG, "Unable to open log file for writting");
	} else {

		uart_init();

		while(1) {
			//ESP_LOGI(TAG, "Log file size %ld", ftell(log_file));

			vTaskDelay(10 / portTICK_PERIOD_MS);
			gpio_set_level(GPIO_HEARTBEAT_LED, 0);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			gpio_set_level(GPIO_HEARTBEAT_LED, 1);
		}
	}
	fclose(log_file);
}