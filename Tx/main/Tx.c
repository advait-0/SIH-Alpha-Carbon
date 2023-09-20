#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_now.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEFAULT_VREF 1100

static const char *TAG = "READINGS";

// Define the sender and receiver MAC addresses
uint8_t unicastAddress[] = {0xC4,0xDD,0x57,0x5B,0xF9,0xA4};            //   Receiver MAC Address: 44:17:93:E6:35:08
uint8_t receiverAddress[] = {0xC4,0xDD,0x57,0x5B,0xF9,0xA4};           // hub address is C4:DD:57:5B:F9:A4

// Callback function to handle ESP-NOW sending status
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        ESP_LOGI(TAG, "Message sent successfully");
    } else {
        ESP_LOGE(TAG, "Message sending failed");
    }
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));  
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Initialize ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    esp_now_register_send_cb(OnDataSent);

    // Add the receiver
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));

    // Configuration for the ADC
    esp_err_t err;
    adc1_config_width(ADC_WIDTH_BIT_12);  // Configure ADC width to 12 bits
    err = adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);          //       Configure ADC channel attenuation
    
    if (err != ESP_OK) {
        // Handle error here
        ESP_LOGE(TAG,"Error in adc attenuation");
    } 
    
    while (1) {
        // Read analog sensor data
        uint32_t sensor_value = adc1_get_raw(ADC1_CHANNEL_0);  // Read sensor on ADC channel 0

        // Convert the sensor value to a string
        
        //char readings_str[100];
        char sensor_data_str[20];  // Adjust buffer size as needed
        snprintf(sensor_data_str, sizeof(sensor_data_str), "%d", sensor_value);
        
        // Send the sensor data
        esp_err_t result = esp_now_send(unicastAddress, (uint8_t *)sensor_data_str, strlen(sensor_data_str));
        if (result == ESP_OK) {
            ESP_LOGI(TAG, "Sensor data sent successfully");
        } else {
            ESP_LOGE(TAG, "Sensor data sending failed");
        }

        // Add a delay to control the data sending rate
        vTaskDelay(pdMS_TO_TICKS(100));  // Adjust the delay time as needed (1 second in this example)
    }
}

