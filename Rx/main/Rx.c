#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "RECEIVER";

uint8_t receiverAddress[] = {0xE8, 0x31, 0xCD, 0xC3, 0xEF, 0x8C}; // Replace with the MAC address of this ESP    Rx: E8:31:CD:C3:EF:8C      Tx: E8:31:CD:C3:B0:00

void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
    // Print received data
    ESP_LOGI(TAG, "Received message: %.*s", len, data);

    // Do something with the received data, if needed
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
    
    // Add the receiver
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));

    // Set up the received callback
    ESP_ERROR_CHECK(esp_now_register_recv_cb(OnDataReceived));

    // Main loop
    //while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "WAITING");
    //}
}

