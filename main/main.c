#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

#include "mpu6050.h"



void app_main(void)
{
    // Bus Config.
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 0,
        .scl_io_num = 22,
        .sda_io_num = 21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    // MPU6050 Config.
    i2c_master_dev_handle_t mpu_handle = init_mpu6050(bus_handle);

    // MPU6050 Gyro Setup
    uint8_t setup_write = 0b11110000;
    uint8_t setup_read = 0;
    uint8_t setup_addr = 0x1B;
    uint8_t setup[3] = {setup_addr, setup_write, setup_read};

    ESP_ERROR_CHECK(i2c_master_transmit(mpu_handle, setup, 2, -1));
    ESP_ERROR_CHECK(i2c_master_transmit_receive(mpu_handle, &setup_addr, 1, &setup[2], 1, -1));

    // Buffers for read data
    uint8_t write = 0x43;
    uint8_t databuf[6] = {0};
    int tracker = 0;
    int x = 0;
    int y = 0;
    int z = 0;

    while(tracker < 100) {
        ESP_ERROR_CHECK(i2c_master_transmit_receive(mpu_handle, &write, 1, databuf, 6, -1));
        
        x += (int16_t)((databuf[0]<<8) | databuf[1]);
        y += (int16_t)((databuf[2]<<8) | databuf[3]);
        z += (int16_t)((databuf[4]<<8) | databuf[5]);
        tracker += 1;

        printf("X: %d\nY: %d\nZ: %d\n", (int16_t)((databuf[0]<<8) | databuf[1]), (int16_t)((databuf[2]<<8) | databuf[3]), 
                                        (int16_t)((databuf[4]<<8) | databuf[5]));

        vTaskDelay(pdMS_TO_TICKS(100));

    }

    x /= tracker;
    y /= tracker;
    z /= tracker;

    while(1) {
        ESP_ERROR_CHECK(i2c_master_transmit_receive(mpu_handle, &write, 1, databuf, 6, -1));
        
        int16_t x_corr = (int16_t)((databuf[0]<<8) | databuf[1]) - x;
        int16_t y_corr = (int16_t)((databuf[2]<<8) | databuf[3]) - y;
        int16_t z_corr = (int16_t)((databuf[4]<<8) | databuf[5]) - z;

        printf("X: %d\nY: %d\nZ: %d\n", x_corr, y_corr, z_corr);
        vTaskDelay(pdMS_TO_TICKS(800));
    }
    

}
