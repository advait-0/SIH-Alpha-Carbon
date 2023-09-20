
#include <iostream>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "esp_wifi.h"
#include "esp_now.h"

#include "jsoncpp/value.h"
#include "jsoncpp/json.h"

#include "esp_firebase/app.h"
#include "esp_firebase/rtdb.h"

#include "wifi_utils.h"

#include "firebase_config.h"
using namespace ESPFirebase;

static const char *TAG = "RECEIVER";

uint8_t receiverAddress[] = {0xE8, 0x31, 0xCD, 0xC3, 0xEF, 0x8C}; // Replace with the MAC address of this ESP
uint8_t transmitterAddress1[] = {0xE8, 0x31, 0xCD, 0xC3, 0xEF, 0x8C}; // MAC address of the first transmitting ESP
uint8_t transmitterAddress2[] = {0xE8, 0x31, 0xCD, 0xC3, 0xB0, 0x00}; // MAC address of the second transmitting ESP
//uint8_t * data_sensor;
const uint8_t * data_sensor;

//uint8_t * data_2;
int length;
//int len_2;
//char * sensor;
const char* sensor ;


void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
    // Check which ESP sent the data based on its MAC address
    //RTDB db = RTDB(&app, DATABASE_URL);// chindi giri
    ESP_LOGI(TAG, "Received message");
 
    if (memcmp(mac_addr, transmitterAddress1, 6) == 0) {
        // Data from the first transmitting ESP
        ESP_LOGI(TAG, "Received message from ESP 1: %.*s", len, data);
        
        data_sensor = data;
        length = len;
        sensor = "sensor_1";
        //Json::Value new_data; 
        //new_data["name"] = "Sensor 1";
        //new_data["age"] = data;
        //new_data["random_float"] = len;

       //db.putData("sensor1", new_data);
        //std::string json_str = R"({"name": "Sensor_1", "value": 20, "len": 8.56})";
        //db.putData("/sensor1", json_str.c_str()); 
        // Process data from ESP 1 here
    } else if (memcmp(mac_addr, transmitterAddress2, 6) == 0) {
        // Data from the second transmitting ESP
        ESP_LOGI(TAG, "Received message from ESP 2: %.*s", len, data);
        data_sensor = data;
        length = len;
        sensor = "sensor_2";
        // Process data from ESP 2 here
    } else {
        // Data from an unknown source
        ESP_LOGW(TAG, "Received message from unknown source");
    }
}

extern "C" void app_main(void)
{
    wifiInit(SSID, PASSWORD);  // blocking until it connects

    // Config and Authentication
    user_account_t account = {USER_EMAIL, USER_PASSWORD};

    FirebaseApp app = FirebaseApp(API_KEY);

    app.loginUserAccount(account);

    RTDB db = RTDB(&app, DATABASE_URL);

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
    std::string buf="";
    int c = 0;
    // Set up the received callback
    while(1){
    int i = 1;
    
    //char* no = (char *) i;
    int data_1 = 4092;
    //string read = "reading";
    //buf = sprintf(buf,"reading%d",i); //reading 4092
    Json::Value new_data_1; 
    new_data_1["name"] = "sensor_1";
    new_data_1["data"] = data_1;
    data_1 = data_1 - 3;
    new_data_1["random_float"] = 12;
    db.putData(std::string(c), new_data_1);
    vTaskDelay(500/portTICK_PERIOD_MS);
    int  data_2 = 2091;
    Json::Value new_data_2; 
    new_data_2["name"] = "sensor_2";
    new_data_2["data"] = data_2;
    data_2 = data_2 + 4;
    new_data_2["random_float"] = 12;
    char str[20]; // Allocate a character array to hold the string representation

    sprintf(str, "%d", c);
    db.putData(str, new_data_2);
    i++;
    c++;
    vTaskDelay(500/portTICK_PERIOD_MS);
    }

    //db.putData("reading1", new_data_1);
    
    //ESP_ERROR_CHECK(esp_now_register_recv_cb(OnDataReceived));
    
    //Json::Value new_data_1; 
    //new_data_1["name"] = "sensor_1";
    //new_data_1["data"] = "4095";
    //new_data_1["random_float"] = 2;

    //db.putData("reading1", new_data_1);
    // R"()" allow us to write string as they are without escaping the characters with backslash

    // We can put a json str directly at /person1
    //std::string json_str = R"({"name": "sensor_2", "data": 2092, "len": 12})";
    //db.putData("/person1", json_str.c_str()); 
    // vTaskDelay(500/portTICK_PERIOD_MS);

    // We can parse the json_str and access the members and edit them
   // Json::Value data;
    //Json::Reader reader; 
    //reader.parse(json_str, data);

   // std::string madjid_name = data["name"].asString();  // convert value to primitives (read jsoncpp docs for more of these)

    // ESP_LOGI("MAIN", "name: %s", madjid_name.c_str());   

   // data["name"] = "sensor_1";

    // ESP_LOGI("MAIN", "edited name from %s to: %s", madjid_name.c_str(), data["name"].asString().c_str());   

    //.data["data"] = 4095;
    //data["random_float"] = 12;
    
    // put json object directly
    //db.putData("/person2", data);   

    // vTaskDelay(500/portTICK_PERIOD_MS);
    // Construct a new json object manually
    //Json::Value new_data; 
    //new_data["name"] = "sensor_2";
    //new_data["data"] = 2000;
    //new_data["random_float"] = 2;

    //db.putData("person3", new_data);
    // vTaskDelay(500/portTICK_PERIOD_MS);

    // Edit person2 data in the database by patching
    //data["data"] = 4003;
    //db.patchData("person2", data);
    Json::Value root = db.getData("person3"); // retrieve person3 from database, set it to "" to get entire database

    Json::FastWriter writer;
    std::string person3_string = writer.write(root);  // convert it to json string

    ESP_LOGI("MAIN", "person3 as json string: \n%s", person3_string.c_str());

    // You can also print entire Json Value object with std::cout with converting to string 
    // you cant print directly with printf or LOGx because Value objects can have many type. << is overloaded and can print regardless of the type of the Value
    std::cout << root << std::endl;

    // print the members (Value::Members is a vector)
    Json::Value::Members members = root.getMemberNames();  
    for (const auto& member : members)
    {
        std::cout << member << ", ";
    }
    std::cout << std::endl;


    db.deleteData("person3"); // delete person3
    root = db.getData("person3"); // retrieve person3 from database, this time it will be null because person3 doesnt exist in database
    std::cout << root << std::endl;
    


}

