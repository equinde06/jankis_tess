/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>  // Incluir la biblioteca para usar abort()
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

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
    int counter = 0;
    const int error_threshold = 8;  // Numero de iteraciones antes de generar un error

    while (1) {
        int valor = 10;
        print_serial_log("El valor es: %d", valor);
        print_serial_log_warning("Esto es una advertencia.");
        print_serial_log_error("Esto es un error");


        /*if (counter >= error_threshold) {
            print_serial_log_error("¡Error generado después de %d iteraciones!", counter);
            break;  // Detener la ejecución del programa
        } else {
            print_serial_log("Contador actual: %d", counter);
        }

        counter++;*/
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}

