#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <sys/param.h>
#include <esp_ota_ops.h>
#include <time.h>
#include "esp_flash.h"
#include "esp_log.h"
#include "http.h"

static const char *TAG = "http";

static httpd_handle_t http_server = NULL;

esp_err_t index_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "application/octet-stream");

	FILE* fd = fopen("/data/log.txt", "r");

	if (fd == NULL) {
		ESP_LOGI(TAG, "Failed to open log file");
		httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
		return ESP_FAIL;
	}

	/* Retrieve the pointer to scratch buffer for temporary storage */
	char *chunk =  malloc(SCRATCH_BUFSIZE);
	size_t chunksize;

	//fseek(fd, 0, SEEK_END);
	//sprintf(chunk, "%ld", ftell(fd));
	//httpd_resp_set_hdr(req, "Content-Length", chunk);
	//fseek(fd, 0, SEEK_SET);

	do {
		/* Read file in chunks into the scratch buffer */
		chunksize = fread(chunk, 1, SCRATCH_BUFSIZE, fd);

		if (chunksize > 0) {
			/* Send the buffer contents as HTTP response chunk */
			if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
				fclose(fd);
				ESP_LOGE(TAG, "File sending failed!");
				/* Abort sending file */
				httpd_resp_sendstr_chunk(req, NULL);
				/* Respond with 500 Internal Server Error */
				httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
				return ESP_FAIL;
			}
		}

		/* Keep looping till the whole file is sent */
	} while (chunksize != 0);

	free(chunk);
	fclose(fd);
	ESP_LOGI(TAG, "File sending complete");

	httpd_resp_set_hdr(req, "Connection", "close");

	httpd_resp_send_chunk(req, NULL, 0);

	return ESP_OK;
}

httpd_uri_t index_get = {
	.uri		= "/",
	.method		= HTTP_GET,
	.handler	= index_get_handler,
	.user_ctx	= NULL
};

esp_err_t http_server_init(void)
{
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.max_uri_handlers = 10;

	ESP_LOGI(TAG, "Starting HTTP server on port %d", config.server_port);

	if (httpd_start(&http_server, &config) == ESP_OK) {
		httpd_register_uri_handler(http_server, &index_get);
	}

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}

esp_err_t http_server_stop(void)
{
	int ret;

	ESP_LOGI(TAG, "Stopping HTTP server");

	if (http_server) {
		ret = httpd_stop(http_server);
	} else {
		ret = ESP_ERR_INVALID_ARG;
	}
	return ret;
}