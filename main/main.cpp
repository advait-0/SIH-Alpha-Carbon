
#include <iostream>
#include <cstdint>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "jsoncpp/value.h"
#include "jsoncpp/json.h"

#include "esp_firebase/app.h"
#include "esp_firebase/rtdb.h"

#include "wifi_utils.h"

#include "firebase_config.h"
using namespace ESPFirebase;
extern "C" void app_main(void)
{
    wifiInit(SSID, PASSWORD);  // blocking until it connects

    // Config and Authentication
    user_account_t account = {USER_EMAIL, USER_PASSWORD};

    FirebaseApp app = FirebaseApp(API_KEY);

    app.loginUserAccount(account);

    RTDB db = RTDB(&app, DATABASE_URL);

    

    // R"()" allow us to write string as they are without escaping the characters with backslash

    // We can put a json str directly at /person1
    std::string json_str = R"({"name": "sensor1", "data":1686 })";
    // db.putData("1", json_str.c_str()); 
    // vTaskDelay(500/portTICK_PERIOD_MS);

    // We can parse the json_str and access the members and edit them
    Json::Value data;
    Json::Reader reader; 
    reader.parse(json_str, data);

    std::string madjid_name = data["name"].asString();  // convert value to primitives (read jsoncpp docs for more of these)

    // ESP_LOGI("MAIN", "name: %s", madjid_name.c_str());   

    data["name"] = "sensor2";

    // ESP_LOGI("MAIN", "edited name from %s to: %s", madjid_name.c_str(), data["name"].asString().c_str());   

    data["data"] = 3232;
    
    
    // put json object directly
    // db.putData("7", data);   
    
    // std::string json_str_1 = R"({"name": "sensor1", "data": 1688})";
    // db.putData("2", json_str_1.c_str()); 
    
    // std::string json_str_2 = R"({"name": "sensor2", "data": 3143})";
    // db.putData("3", json_str_2.c_str()); 
    
    // std::string json_str_3 = R"({"name": "sensor1", "data": 1667})";
    // db.putData("4", json_str_3.c_str()); 
    
    // std::string json_str_4 = R"({"name": "sensor2", "data": 3152})";
    // db.putData("5", json_str_4.c_str()); 

    // vTaskDelay(500/portTICK_PERIOD_MS);
    // Construct a new json object manually
    static const char *TAG = "READINGS";

    esp_err_t err;
    adc1_config_width(ADC_WIDTH_BIT_12);  // Configure ADC width to 12 bits
    err = adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);          //       Configure ADC channel attenuation
    
    if (err != ESP_OK) {
        // Handle error here
        ESP_LOGE(TAG,"Error in adc attenuation");
    } 


    
    for (int counter = 0; counter <= 4; counter++)
    {
        
        uint32_t sensor_value = adc1_get_raw(ADC1_CHANNEL_0); 

        ESP_LOGI(TAG,"sensor readings are %d",sensor_value);
        vTaskDelay(pdMS_TO_TICKS(500));

        int intValue = static_cast<int>(sensor_value);

        Json::Value new_data; 
        new_data["Sensor"] = "Sensor 1";
        new_data["data"] = intValue;
        new_data["Length"] = 12;
        
        //std::string str = std::to_string(counter);
        std::string str = std::to_string(counter);
        const char* char_ptr = str.c_str();

        db.putData(char_ptr, new_data);
        

    }
  
    //db.putData("person3", new_data);
    // vTaskDelay(500/portTICK_PERIOD_MS);

    // Edit person2 data in the database by patching
    // data["age"] = 23;
    // //db.patchData("person2", data);
    // Json::Value root = db.getData("person3"); // retrieve person3 from database, set it to "" to get entire database

    // Json::FastWriter writer;
    // std::string person3_string = writer.write(root);  // convert it to json string

    // ESP_LOGI("MAIN", "person3 as json string: \n%s", person3_string.c_str());

    // // You can also print entire Json Value object with std::cout with converting to string 
    // // you cant print directly with printf or LOGx because Value objects can have many type. << is overloaded and can print regardless of the type of the Value
    // std::cout << root << std::endl;

    // // print the members (Value::Members is a vector)
    // Json::Value::Members members = root.getMemberNames();  
    // for (const auto& member : members)
    // {
    //     std::cout << member << ", ";
    // }
    // std::cout << std::endl;



    // db.deleteData("person2"); // delete person3
    // root = db.getData("person3"); // retrieve person3 from database, this time it will be null because person3 doesnt exist in database
    // std::cout << root << std::endl;


}
