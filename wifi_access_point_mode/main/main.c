#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <esp_http_server.h>

#define username "green lantern"
#define wifi_password "Mogo_doesn't_socialize"


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
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
           ESP_LOGI("WIFI", "MAC address of connected device is " MACSTR,MAC2STR(event->mac));
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
           ESP_LOGI("WIFI", "MAC address of the device disconnected is " MACSTR,MAC2STR(event->mac));
    }
}


void wifi_init_ap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

    wifi_config_t set_wifi = {
        .ap = {
            .ssid = username,
            .ssid_len = strlen(username),
            .channel = 1,
            .password = wifi_password,
            .max_connection = 3,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &set_wifi)); 
    ESP_ERROR_CHECK(esp_wifi_start());

    webserver();
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
    wifi_init_ap();
}