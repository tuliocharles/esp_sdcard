#ifndef _esp_sdcard_H_
#define _esp_sdcard_H_

#include "esp_err.h"
#include "esp_check.h"
//#include <stdbool.h>
#include <stdio.h>
//#include <string.h>

typedef struct esp_sdcard_t *esp_sdcard_handle_t;

typedef struct{
    int mosi_io;  
    int miso_io;  
    int clk_io;  
    int cs_io;  
} esp_sdcard_config_t;



esp_err_t init_esp_sdcard(esp_sdcard_config_t *config, esp_sdcard_handle_t *handle);

esp_err_t esp_sdcard_mount(const char *mount_point, esp_sdcard_handle_t handle);

esp_err_t esp_sdcard_unmount(esp_sdcard_handle_t handle);


// File operation functions

esp_err_t esp_sdcard_write(const char *path, const void *data, size_t len, esp_sdcard_handle_t handle);

esp_err_t esp_sdcard_read(const char *path, void *buffer, size_t max_len, size_t *out_len, esp_sdcard_handle_t handle);

esp_err_t esp_sdcard_exists(const char *path, bool *exists, esp_sdcard_handle_t handle);

esp_err_t esp_sdcard_remove(const char *path,  esp_sdcard_handle_t handle);

esp_err_t esp_sdcard_rename(const char *old_path, const char *new_path, esp_sdcard_handle_t handle);

void func(void); //












#endif