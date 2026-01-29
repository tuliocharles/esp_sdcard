#include <stdio.h>
#include "esp_sdcard.h"


#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sd_test_io.h"

#define MAX_CHAR_SIZE    64

static const char *TAG = "SDCARD";

typedef struct esp_sdcard_t esp_sdcard_t;

struct esp_sdcard_t
{
    spi_bus_config_t bus;
    sdspi_device_config_t slot;
    sdmmc_host_t host;
    sdmmc_card_t *card;
    esp_vfs_fat_sdmmc_mount_config_t mount;
    char mount_point[32];
};



esp_err_t esp_sdcard_mount(const char *mount_point, esp_sdcard_handle_t handle)
{

    esp_err_t ret = ESP_OK; ///
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    handle->mount = (esp_vfs_fat_sdmmc_mount_config_t){
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    snprintf(handle->mount_point, sizeof(handle->mount_point), "%s", mount_point);

    ESP_LOGI(TAG, "Initializing SD card");

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(handle->mount_point, &handle->host, &handle->slot, &handle->mount, &handle->card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, handle->card);
    // Use POSIX and C standard library functions to work with files.
    return ret;
   
}

esp_err_t esp_sdcard_unmount(esp_sdcard_handle_t handle)
{
    esp_err_t ret = ESP_OK;
    // Unmount the filesystem and disable the SD card
    ESP_LOGI(TAG, "Unmounting filesystem");
    ret = esp_vfs_fat_sdcard_unmount(handle->mount_point, handle->card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unmount filesystem (%s)", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Filesystem unmounted");

    return ret;
}

esp_err_t init_esp_sdcard(esp_sdcard_config_t *config, esp_sdcard_handle_t *handle)
{
    esp_err_t ret = ESP_OK;
    esp_sdcard_t *esp_sdcard = NULL;
    ESP_GOTO_ON_FALSE(config && handle, ESP_ERR_INVALID_ARG, err, TAG, "Invalid arguments");
    esp_sdcard = calloc(1, sizeof(esp_sdcard_t));

    if (!esp_sdcard)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for esp_sdcard");
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    
    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz (range 400kHz - 20MHz for SDSPI)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
    esp_sdcard->host = (sdmmc_host_t) SDSPI_HOST_DEFAULT();

    // For SoCs where the SD power can be supplied both via an internal or external (e.g. on-board LDO) power supply.
    // When using specific IO pins (which can be used for ultra high-speed SDMMC) to connect to the SD card
    // and the internal LDO power supply, we need to initialize the power supply first.
   
    esp_sdcard->bus.mosi_io_num = config->mosi_io;
    esp_sdcard->bus.miso_io_num = config->miso_io;
    esp_sdcard->bus.sclk_io_num = config->clk_io;
    esp_sdcard->bus.quadwp_io_num = -1;
    esp_sdcard->bus.quadhd_io_num = -1;
    esp_sdcard->bus.max_transfer_sz = 4000;

    ret = spi_bus_initialize(esp_sdcard->host.slot, &esp_sdcard->bus, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return ret;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    esp_sdcard->slot = (sdspi_device_config_t)SDSPI_DEVICE_CONFIG_DEFAULT();
    esp_sdcard->slot.gpio_cs = config->cs_io;
    esp_sdcard->slot.host_id = esp_sdcard->host.slot;

    

    *handle = esp_sdcard;
    ESP_LOGI(TAG, "sdcard initialized successfully");
    ret =  ESP_OK;
    return ret;
    
err:
    if (esp_sdcard)
    {
        free(esp_sdcard);
        esp_sdcard = NULL;
    }

    ESP_LOGE(TAG, "Failed to initialize adc: %s", esp_err_to_name(ret));
    return ret;
}



esp_err_t esp_sdcard_write(const char *path, const void *data, size_t len, esp_sdcard_handle_t handle)
{
    esp_err_t ret = ESP_OK;

    char full_path[MAX_CHAR_SIZE];
    snprintf(full_path, sizeof(full_path),
         "%s/%s",
         handle->mount_point,
         path);
    
    FILE *f = fopen(full_path, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    size_t written = fwrite(data, 1, len, f);
    if (written != len) {
        ESP_LOGE(TAG, "Failed to write data to file");
        ret = ESP_FAIL;
    }
    fclose(f);
    
    return ret;
}

