#include "esp_system.h"
#include "esp_wifi.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "Sensor";

void app_main() 
{
    
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    printf("Transmitter MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);        

     esp_err_t err;
    adc1_config_width(ADC_WIDTH_BIT_12);  // Configure ADC width to 12 bits
    err = adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);          //       Configure ADC channel attenuation
    
    if (err != ESP_OK) {
        // Handle error here
        ESP_LOGE(TAG,"Error in adc attenuation");
    } 

    while(1)
    {
        uint32_t sensor_value = adc1_get_raw(ADC1_CHANNEL_0); 

        ESP_LOGI(TAG,"sensor readings are %d",sensor_value);



         vTaskDelay(pdMS_TO_TICKS(500));
    }                
}
