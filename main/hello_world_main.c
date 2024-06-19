/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

/*#define CONFIG_EZM_SERVICE_COLECTOR_LOG
[7A] [7A] [7A] [7A]
*/
#ifdef CONFIG_EZM_SERVICE_COLECTOR_LOG
    static const char*          TAG = "colector.c";
    #define print_serial_log(format, ... )              ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, TAG, "[INFO] %s: Line = %d " format "\n", __func__, __LINE__, ##__VA_ARGS__)
    #define print_serial_log_warning(format, ... )      ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN, TAG, "[WARN] %s:  Line = %d " format "\n", __func__, __LINE__, ##__VA_ARGS__)
    #define print_serial_log_error(format, ... )        ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, TAG, "[ERROR] %s:  Line = %d " format "\n", __func__, __LINE__ , ##__VA_ARGS__)
    #define print_serial_log_buffer(data,datalen)       ESP_LOG_BUFFER_HEXDUMP(TAG,data,datalen,ESP_LOG_ERROR);
#else
    #define print_serial_log(format, ... )              printf("[7A] [%d] [%s] [OK] [7A] " format "\n", __LINE__, __func__,  ##__VA_ARGS__)
    #define print_serial_log_warning(format, ... )      printf("[7A] [%d] [%s] [W] [7A] " format "\n", __LINE__, __func__,  ##__VA_ARGS__)
    #define print_serial_log_error(format, ... )        printf("[7A] [%d] [%s] [ERR] [7A] " format "\n", __LINE__, __func__,  ##__VA_ARGS__)
    #define print_serial_log_buffer(data,datalen)       printf("[7A] [%d] [%s] [BUFF] [7A] " format "\n", __LINE__, __func__,  ##__VA_ARGS__)
#endif
/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/

void app_main(void)
{
    print_serial_log_error("Esto es un error");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
