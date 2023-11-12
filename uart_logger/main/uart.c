#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/uart.h"

#include "main.h"
#include "uart.h"

static const char *TAG = "uart";

static QueueHandle_t uart_queue;

void uart_event_task(void *pvParameters)
{
	uart_event_t event;
	size_t recv_buffer_size;
	char *buf = (char *) malloc(UART_BUF_SIZE);

	while(1) {
		if(xQueueReceive(uart_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
			switch(event.type) {
				case UART_DATA:
					break;
				case UART_FIFO_OVF:
					ESP_LOGI(TAG, "RAK FIFO Overflow");
					break;
				case UART_BUFFER_FULL:
					ESP_LOGI(TAG, "RAK Buffer Full");
					break;
				case UART_BREAK:
					ESP_LOGI(TAG, "RAK Recv Break");
					break;
				case UART_PARITY_ERR:
					ESP_LOGI(TAG, "RAK Parity Error");
					break;
				case UART_FRAME_ERR:
					ESP_LOGI(TAG, "RAK Frame Error");
					break;
				case UART_PATTERN_DET:
					uart_get_buffered_data_len(UART_PORT, &recv_buffer_size);

					int pos = uart_pattern_pop_pos(UART_PORT);
					if (pos == -1) {
						uart_flush_input(UART_PORT);
					} else {
						int len = uart_read_bytes(UART_PORT, buf, pos+1, 100 / portTICK_PERIOD_MS);
						if (len > 0 ) {
							buf[len] = 0;
							process_received_data(buf, len);
						}
					}
					break;
				default:
					ESP_LOGI(TAG, "RAK Unknown UART event type: %d", event.type);
					break;
			}
		}
	}
	free(buf);
	buf = NULL;
	vTaskDelete(NULL);
}

void uart_init(void)
{
	ESP_LOGI(TAG, "Initialsing UART on Port %d, Baudrate %d", UART_PORT, UART_BAUD_RATE);

	uart_config_t uart_config = {
		.baud_rate = UART_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
	};

	// Install UART driver
	ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUF_SIZE * 2, 0, 20, &uart_queue, 0));

	// Configure UART parameters
	ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));

	// Set UART pins
	ESP_ERROR_CHECK(uart_set_pin(UART_PORT, GPIO_UART_TXD, GPIO_UART_RXD, -1, -1));

	// Set to regular UART mode
	ESP_ERROR_CHECK(uart_set_mode(UART_PORT, UART_MODE_UART));

	// Set UART pattern detect function.
	uart_enable_pattern_det_baud_intr(UART_PORT, '\n', 1, 9, 0, 0);

	//Reset the pattern queue length to record at most 20 pattern positions.
	uart_pattern_queue_reset(UART_PORT, 20);

	//Create a task to handler UART event from ISR
	xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 12, NULL);
}