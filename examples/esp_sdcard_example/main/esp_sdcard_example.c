#include <stdio.h>
#include "esp_sdcard.h"


void app_main(void)
{

    esp_sdcard_config_t config = {
        .mosi_io = 15,
        .miso_io = 2,
        .clk_io = 14,
        .cs_io = 13
    };

    esp_sdcard_handle_t esp_sdcard_handle;
    
    esp_err_t ret = init_esp_sdcard(&config, &esp_sdcard_handle);
    if (ret != ESP_OK) {
        printf("Failed to initialize SD card: %s\n", esp_err_to_name(ret));
        return;
    }
    printf("SD card initialized successfully\n");

    ret = esp_sdcard_mount("/sdcard", esp_sdcard_handle);
    if (ret != ESP_OK) {
        printf("Failed to mount SD card: %s\n", esp_err_to_name(ret));
        
    } else {
         printf("SD card mounted successfully\n");
    }
    
    ret = esp_sdcard_unmount(esp_sdcard_handle);
    if (ret != ESP_OK) {
        printf("Failed to unmount SD card: %s\n", esp_err_to_name(ret));
    } else {
        printf("SD card unmounted successfully\n");
    }

    ret = esp_sdcard_mount("/sdcard2", esp_sdcard_handle);
    if (ret != ESP_OK) {
        printf("Failed to mount SD card: %s\n", esp_err_to_name(ret));
        
    } else {
         printf("SD card mounted successfully\n");
    }
    

}
