#ifndef __SD_H__
#define __SD_H__

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/gpio.h"

#define SDMMC_DAT2     12
#define SDMMC_D1       4
#define SDMMC_D3       13
#define SDMMC_CMD      15
#define SDMMC_D0       2
#define SDMMC_CLK      14

#define SD_SCLK        SDMMC_CLK
#define SD_CS          SDMMC_CMD
#define SD_MOSI        SDMMC_D3
#define SD_MISO        SDMMC_DAT2

esp_err_t init_sdmmc_idf(bool use1Bit);

#endif