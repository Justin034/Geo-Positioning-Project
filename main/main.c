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
    // Bus Config
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

    i2c_master_dev_handle_t mpu_handle = init_mpu6050(bus_handle);

    uint8_t write = 0x43;
    uint8_t databuf[6] = {0};

    while(1) {
        ESP_ERROR_CHECK(i2c_master_transmit_receive(mpu_handle, &write, 1, databuf, 6, -1));
        
        uint16_t x = (uint16_t)((databuf[0]<<8) | databuf[1]);
        uint16_t y = (uint16_t)((databuf[2]<<8) | databuf[3]);
        uint16_t z = (uint16_t)((databuf[4]<<8) | databuf[5]);

        printf("X: %d\nY: %d\nZ: %d\n", x, y, z);
        vTaskDelay(pdMS_TO_TICKS(800));
    }
    

}
