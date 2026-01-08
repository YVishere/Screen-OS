#include "SD_intf.h"

static const char *MOUNT_POINT = "/sdcard";

esp_err_t init_sdmmc_idf(bool use1Bit) {
    // Recommend external pull-ups; enable internal as a helper
    gpio_pullup_en(GPIO_NUM_2);
    gpio_pullup_en(GPIO_NUM_4);
    gpio_pullup_en(GPIO_NUM_12);
    gpio_pullup_en(GPIO_NUM_13);
    gpio_pullup_en(GPIO_NUM_15);

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_DEFAULT; // e.g., 20000–40000; tune per board

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = use1Bit ? 1 : 4; // 4-bit requires D1/D2/D3 wired

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    
    if (ret != ESP_OK) {
        printf("Failed to mount SDMMC: %s\n", esp_err_to_name(ret));
        return ret;
    }

    sdmmc_card_print_info(stdout, card);
    return ESP_OK;
}