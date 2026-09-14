#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAIN";
static int counter = 0;
void my_first_task(void *pointer)
{
    while(1)
    {
        ESP_LOGI(TAG, "Hello from ESP32! Counter: %d", counter);
        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void app_main(void)
{
    xTaskCreate(my_first_task,"teh first task",2048,NULL,3,NULL);
    printf("the task starting......");
}