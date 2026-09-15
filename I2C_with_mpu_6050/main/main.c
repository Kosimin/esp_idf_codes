#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#define SDA GPIO_NUM_21
#define SCL GPIO_NUM_22

#define MPU_address 0x68

void app_main(void)
{
    i2c_master_bus_config_t bus_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_NUM_0,
    .scl_io_num = SCL,
    .sda_io_num = SDA,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));
    i2c_device_config_t i2c_device = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = MPU_address,
    .scl_speed_hz = 400000,
};
i2c_master_dev_handle_t device_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle,&i2c_device,&device_handle));

uint8_t wakeup_cmd[2] = {0x6B, 0x00};
ESP_ERROR_CHECK(i2c_master_transmit(device_handle,wakeup_cmd,sizeof(wakeup_cmd),-1));

while(1)
{
    uint8_t reg = 0x3B;
    uint8_t data[6];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(device_handle,&reg,sizeof(reg),data,sizeof(data),-1));

    int16_t x = ((data[0] << 8) | data[1]);
    int16_t y = ((data[2] << 8) | data[3]);
    int16_t z = ((data[4] << 8) | data[5]);

    float ax = x / 16384.0f;
    float ay = y / 16384.0f;
    float az = z / 16384.0f;


    printf("Accelerometer: ax=%.2f, ay=%.2f, az=%.2f\n", ax, ay, az);
    vTaskDelay(pdMS_TO_TICKS(500));

}
}