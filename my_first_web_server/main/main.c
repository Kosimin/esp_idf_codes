#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <esp_http_server.h>

#define username "green lantern"
#define wifi_password "09876543"


esp_err_t root(httpd_req_t *req)
{
    const char mesg[] = "Hlo, I am Adihtya...";
    httpd_resp_send(req, mesg, sizeof(mesg) - 1);
    return ESP_OK;
}


esp_err_t hello(httpd_req_t *req)
{
    const char mesg[] = "hlo,world";
    httpd_resp_send(req, mesg, sizeof(mesg) - 1);
    return ESP_OK;
}


void webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t uri1 = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root,
        };
        httpd_uri_t uri2 = {
            .uri = "/hello",
            .method = HTTP_GET,
            .handler = hello,
        };
        httpd_register_uri_handler(server, &uri1);
        httpd_register_uri_handler(server, &uri2);
        return;
    }
    printf("no webpage available\n");
    return;
}


void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI("WIFI", "Retrying to connect to the AP");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("WIFI", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        webserver();
    }
}


void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));

    esp_event_handler_instance_t wifi_event;
    esp_event_handler_instance_t ip_event;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &wifi_event));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &ip_event));

    wifi_config_t set_wifi = {
        .sta = {
            .ssid = username,
            .password = wifi_password,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &set_wifi)); 
    ESP_ERROR_CHECK(esp_wifi_start());
}


void app_main(void)
{
    esp_err_t nvs = nvs_flash_init();
    if (nvs != ESP_OK)
    {
        ESP_LOGE("NVS", "NVS is not initialized: %s", esp_err_to_name(nvs));
        return;
    }
    
    printf("nvs is initilized wifi is starting \n");
    wifi_init_sta();
}