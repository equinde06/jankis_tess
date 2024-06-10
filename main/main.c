#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ina219.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/uart.h"
#include <port.h>
#include <string.h>
#include <mqtt_client.h>
#include <esp_wifi.h>
#include "esp_system.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"

/* Definiciones de wifi y broker mosquitto*/
#define WIFI_SSID			"EASYMETERING 2.4"
#define WIFI_PASS 			"ezm12345"
#define MQTT_BROKER_URI		"mqtt://mqtt.eclipseprojects.io/"
#define MQTT_USER 			"ezm"
#define MQTT_PASSWORD 		"ezm12345"


#define I2C_ADDR 0X40
struct ina219_dev  dev ;
struct ina219_setting set;
float bus_voltage, current, power;
char json_data[100];
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
static const char *TAG = "wifi_mqtt";static esp_mqtt_client_handle_t client;


static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*Lectura de datos del sensor INA219*/
void task (void *pvParameters){
	   uart_config_t uart_config = {
	        .baud_rate = 115200,
	        .data_bits = UART_DATA_8_BITS,
	        .parity    = UART_PARITY_DISABLE,
	        .stop_bits = UART_STOP_BITS_1,
	        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	    };
	   uart_param_config(UART_NUM_0, &uart_config);
	   memset(&dev, 0, sizeof(struct ina219_dev));
	   memset(&set, 0, sizeof(struct ina219_setting));
	   dev.chip_id = I2C_ADDR;
	   dev.read = port_i2c_read;
	   dev.write = port_i2c_write;
	   printf("Configurando INA216\n");
	   ina219_conf(&dev,&set,BUS_RANGE_32V,GAIN_0_125,ADC_12BIT,ADC_12BIT,MODE_CONT_SHUNT_BUS);
	   printf("Iniciando INA216\n");
	   ina219_init(&dev);
	   while(1){
	        ina219_voltage(&dev, &bus_voltage);
	        ina219_current(&dev,&set, &current);
	        ina219_power(&dev,&set, &power);
	        //printf("timestamp:%f, voltage: %fV,corriente: %fA, potencia: %fW\n",esp_timer_get_time()/1000000.0,bus_voltage/1000.0,current,power);
	        vTaskDelay(1500 / portTICK_PERIOD_MS);

	   }
}

/*Manejador de eventos del wifi*/
static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            ESP_LOGI(TAG, "got ip:%s\n",
                     ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}
/*Funcion de inicio del wifi*/
void wifi_init_sta() {
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
             WIFI_SSID, WIFI_PASS);
}
/*Manejador de eventos del wifi*/
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    	while(1){
    	sprintf(json_data, "{\"Voltage\": %1"
    			".4f, \"Current\": %.5f, \"Power\": %.5f}",
    	                bus_voltage/1000.0, current, power);
	    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "sensor/voltage",json_data, 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        vTaskDelay(1500 / portTICK_PERIOD_MS);
    	}
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
    	while(1){
    	sprintf(json_data, "{\"Voltage\": %1"
    			".4fV, \"current\": %.5fmA, \"Power\": %.5fmW}",
    	                bus_voltage/1000.0, current, power);
	    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "sensor/voltage",json_data, 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        vTaskDelay(1500 / portTICK_PERIOD_MS);
    	}
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

/*Funcion de inicio del protocolo mqtt*/
static void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URI,
		.username = MQTT_USER,
		.password= MQTT_PASSWORD,
    };


    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

/*Funcion main*/
void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();    // Esperar a que se conecte al WiFi
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    // Iniciar el cliente MQTT
    printf("Iniciando I2C\n");
    port_i2c_init(&dev);
    xTaskCreate(task,"example",configMINIMAL_STACK_SIZE *8, NULL,5, NULL);
    mqtt_app_start();
}

