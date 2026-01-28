#ifndef _esp_sdcard_H_
#define _esp_sdcard_H_

#include "esp_err.h"
#include "esp_check.h"

typedef struct esp_sdcard_t *esp_sdcard_handle_t;

typedef struct{
    int mosi_io;  
    int miso_io;  
    int clk_io;
    int cs_io;
} esp_sdcard_config_t;



esp_err_t init_esp_sdcard(esp_sdcard_config_t *config, esp_sdcard_handle_t *handle);



void func(void);












#endif