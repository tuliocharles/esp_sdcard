#include <stdio.h>
#include "esp_sdcard.h"

void app_main(void)
{

    esp_sdcard_config_t config = {
        .mosi_io = 15,
        .miso_io = 2,
        .clk_io = 14,
        .cs_io = 13};

    esp_sdcard_handle_t esp_sdcard_handle;

    esp_err_t ret = init_esp_sdcard(&config, &esp_sdcard_handle);
    if (ret != ESP_OK)
    {
        printf("Failed to initialize SD card: %s\n", esp_err_to_name(ret));
        return;
    }
    printf("SD card initialized successfully\n");

    ret = esp_sdcard_mount("/sdcard", esp_sdcard_handle);
    if (ret != ESP_OK)
    {
        printf("Failed to mount SD card: %s\n", esp_err_to_name(ret));
    }
    else
    {
        printf("SD card mounted successfully\n");
    }

    ret = esp_sdcard_unmount(esp_sdcard_handle);
    if (ret != ESP_OK)
    {
        printf("Failed to unmount SD card: %s\n", esp_err_to_name(ret));
    }
    else
    {
        printf("SD card unmounted successfully\n");
    }

    ret = esp_sdcard_mount("/teste", esp_sdcard_handle);
    if (ret != ESP_OK)
    {
        printf("Failed to mount SD card: %s\n", esp_err_to_name(ret));
        //
    }
    else
    {
        printf("SD card mounted successfully\n");
    }

    if (esp_sdcard_exists("data1.txt", esp_sdcard_handle))
    {
        printf("File data.txt exists on SD card\n");
        esp_sdcard_append_line("data1.txt", "ToToToToTo_________asdfe", esp_sdcard_handle);
    }
    else
    {
        printf("File data.txt does not exist on SD card\n");
        esp_sdcard_write_line("data1.txt", "xxxxTxxxxxTcccccT", esp_sdcard_handle);
    }

    char buffer[64];
    size_t out_len;
    esp_sdcard_read("data1.txt", buffer, 64, &out_len, esp_sdcard_handle);
    buffer[out_len] = '\0'; // Null-terminate the string
    printf("Read from SD card: %s\n", buffer);

    esp_sdcard_list_files("/", esp_sdcard_handle);

    esp_sdcard_read_lines("data1.txt", esp_sdcard_handle);

    esp_sdcard_open_log("log.txt", esp_sdcard_handle);

    for(int i = 0; i < 3; i++) {
        char log_line[64];
        snprintf(log_line, sizeof(log_line), "Log line %d", i);
        esp_sdcard_write_log(log_line, esp_sdcard_handle);
    }
    
    esp_sdcard_close_log(esp_sdcard_handle);

    esp_sdcard_read_lines("log.txt", esp_sdcard_handle);
    
    // Unmount the SD card
    ret = esp_sdcard_unmount(esp_sdcard_handle);
    }
