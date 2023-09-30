#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <sys/param.h>
#include <esp_ota_ops.h>
#include <time.h>
#include "esp_flash.h"
#include "esp_log.h"

static const char *TAG = "http";

static httpd_handle_t http_server = NULL;

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[] asm("_binary_style_css_end");

esp_err_t index_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) index_html_start, index_html_end - index_html_start);
	return ESP_OK;
}

esp_err_t css_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) style_css_start, style_css_end - style_css_start);
	return ESP_OK;
}

httpd_uri_t index_get = {
	.uri		= "/",
	.method		= HTTP_GET,
	.handler	= index_get_handler,
	.user_ctx	= NULL
};

httpd_uri_t ccs_get = {
	.uri		= "/style.css",
	.method		= HTTP_GET,
	.handler	= css_get_handler,
	.user_ctx	= NULL
};

esp_err_t http_server_init(void)
{
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.max_uri_handlers = 10;

	ESP_LOGI(TAG, "Starting HTTP server on port %d", config.server_port);

	if (httpd_start(&http_server, &config) == ESP_OK) {
		httpd_register_uri_handler(http_server, &index_get);
		httpd_register_uri_handler(http_server, &ccs_get);
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