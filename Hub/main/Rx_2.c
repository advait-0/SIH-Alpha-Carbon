#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "RECEIVER";

uint8_t receiverAddress[] = {0xC4, 0xDD, 0x57, 0x5B, 0xFA, 0x70}; // Replace with the MAC address of this ESP
uint8_t transmitterAddress1[] = {0xC4, 0xDD, 0x57, 0x5B, 0xFA, 0x70}; // MAC address of the first transmitting ESP C4:DD:57:5B:FA:70
uint8_t transmitterAddress2[] = {0x44, 0x17, 0x93, 0xE6, 0x36, 0xBB}; // MAC address of the second transmitting ESP 44:17:93:E6:36:B8
uint8_t * data_1;
uint8_t * data_2;
int len_1;
int len_2;
char * sensor;

void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
    // Check which ESP sent the data based on its MAC address
    if (memcmp(mac_addr, transmitterAddress1, 6) == 0) {
        // Data from the first transmitting ESP
        ESP_LOGI(TAG, "Received message from ESP 1: %.*s", len, data);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGE("Firebase", "Data published :");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        //data_1 = data;
        //len_1 = len;
        //sensor = "sensor_1";
       
        // Process data from ESP 1 here
    } else if (memcmp(mac_addr, transmitterAddress2, 6) == 0) {
        // Data from the second transmitting ESP
        ESP_LOGI(TAG, "Received message from ESP 2: %.*s", len, data);
        //data_1 = data;
        //len_1 = len;
        //sensor = "sensor_2";
        // Process data from ESP 2 here
    } else {
        // Data from an unknown source
        ESP_LOGW(TAG, "Received message from ESP 2: %.*s", len, data);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGE("Firebase", "Data published :");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
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
    
    // Add the receivers for the two transmitting ESPs
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));

    // Receiver for ESP 1
    memcpy(peerInfo.peer_addr, transmitterAddress1, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));

    // Receiver for ESP 2
    memcpy(peerInfo.peer_addr, transmitterAddress2, 6);
    ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));

    // Set up the received callback
    ESP_ERROR_CHECK(esp_now_register_recv_cb(OnDataReceived));

    // Main loop
    while (1) {
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "WAITING");
    }
}

