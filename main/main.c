#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
    
}
