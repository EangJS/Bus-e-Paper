#include "draw.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "DRAW"

extern UBYTE *BlackImage;

int get_time_difference(const char *target_time_str) {
    // Parse the target time string (format: "2024-11-18T02:58:24+08:00")
    struct tm target_tm = {0};
    int utc_offset_hour = 0, utc_offset_minute = 0;
    sscanf(target_time_str, "%4d-%2d-%2dT%2d:%2d:%2d+%2d:%2d",
           &target_tm.tm_year, &target_tm.tm_mon, &target_tm.tm_mday,
           &target_tm.tm_hour, &target_tm.tm_min, &target_tm.tm_sec,
           &utc_offset_hour, &utc_offset_minute);

    // Adjust fields for `struct tm`
    target_tm.tm_year -= 1900; // Year since 1900
    target_tm.tm_mon -= 1;     // Month, 0-based

    // Convert target time to time_t (no need adjust for UTC offset as timezone is set)
    time_t target_time = mktime(&target_tm); //- (utc_offset_hour * 3600 + utc_offset_minute * 60);

    // Get the current time
    time_t now = time(NULL);

    if (now == -1 || target_time == -1) {
        printf("Failed to get time.\n");
        return -1;
    }

    // Calculate the difference in minutes
    double difference_in_minutes = difftime(target_time, now) / 60.0;
    return (int)difference_in_minutes;
}


uint16_t write_bus_stop(cJSON* response, uint16_t idx){     
    cJSON* busStopCode = cJSON_GetObjectItem(response, "BusStopCode");   
    cJSON* services = cJSON_GetObjectItem(response, "Services");
    if (services == NULL){
        ESP_LOGE(TAG,"ERROR Services");        
    }
    uint16_t new_offset = 0; 

    /*Drawing the bus stop code*/
    Paint_DrawString_EN(5, idx+1, busStopCode->valuestring, &Font12, BLACK, WHITE);
    Paint_DrawString_EN(50, idx+1, strcmp(busStopCode->valuestring, "55029") == 0 ? "Seasons Pk" : "Opp Seasons Pk" , &Font12, WHITE, BLACK);

    if (idx == 0){
        /*Drawing the current time*/
        time_t current_time = time(NULL);
        struct tm *local_time = localtime(&current_time);
        if (local_time != NULL) {
            char time_string[64];
            strftime(time_string, sizeof(time_string), "Updated:%H:%M:%S %d-%m-%Y", local_time);
            Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT - 140, idx+3, time_string, &Font8, WHITE, BLACK);
        } else {
            ESP_LOGE(TAG, "Failed to get local time");
        }   
    }
  
    idx += 15;
    
    /*Drawing the bus timings*/
    uint16_t array_size = cJSON_GetArraySize(services); // Get the size of the array
    if (array_size == 0) {
        Paint_DrawString_EN(5, idx, "No services available", &Font12, WHITE, BLACK); 
        return idx + 12; //new start y pos    
    }

    for (uint8_t i = 0; i < array_size; i++) {
        // Get each item in the array using cJSON_GetArrayItem
        cJSON *service = cJSON_GetArrayItem(services, i);
        char arrival_time_1[32];
        char arrival_time_2[32];

        if (service != NULL) {
            char* service_no = cJSON_GetObjectItem(service, "ServiceNo")->valuestring;
            cJSON *next_bus_1 = cJSON_GetObjectItem(service, "NextBus");
            if (next_bus_1 != NULL){                        
                char* estimated_arrival = cJSON_GetObjectItem(next_bus_1, "EstimatedArrival")->valuestring;
                //ESP_LOGI(TAG,"%s",service_no);
                //ESP_LOGI(TAG,"%s",estimated_arrival);
                int arrival_time = get_time_difference(estimated_arrival);
                char* type = cJSON_GetObjectItem(next_bus_1, "Type")->valuestring;
                if (arrival_time < -10) {
                    snprintf(arrival_time_1, sizeof(arrival_time_1), "NA - %s", type); // Assign "NA" when arrival_time < -10
                } 
                else if (arrival_time <= 0) {
                    snprintf(arrival_time_1, sizeof(arrival_time_1), "Arr - %s", type); // Assign "Arr" when arrival_time <= 0
                }
                else {
                    snprintf(arrival_time_1, sizeof(arrival_time_1), "%d mins - %s", arrival_time, type); // Format the arrival_time as a string
                }                
            }                    
            cJSON *next_bus_2 = cJSON_GetObjectItem(service, "NextBus2");
            if (next_bus_2 != NULL){                        
                char* estimated_arrival = cJSON_GetObjectItem(next_bus_2, "EstimatedArrival")->valuestring;
                //ESP_LOGI(TAG,"%s",service_no);
                //ESP_LOGI(TAG,"%s",estimated_arrival);
                int arrival_time = get_time_difference(estimated_arrival);
                char* type = cJSON_GetObjectItem(next_bus_1, "Type")->valuestring;
                if (arrival_time < -10) {
                    snprintf(arrival_time_2, sizeof(arrival_time_2), "NA - %s", type); // Assign "NA" when arrival_time < -10
                } 
                else if (arrival_time <= 0) {
                    snprintf(arrival_time_2, sizeof(arrival_time_2), "Arr - %s", type); // Assign "Arr" when arrival_time <= 0
                }
                else {
                    snprintf(arrival_time_2, sizeof(arrival_time_2), "%d mins - %s", arrival_time, type); // Format the arrival_time as a string
                }
            } 
            char arrival_time_string[128];
            snprintf(arrival_time_string, sizeof(arrival_time_string), "%s : %s | %s", service_no, arrival_time_1, arrival_time_2);
            uint16_t y = idx + i*10.5;
            Paint_DrawString_EN(5, y, arrival_time_string, &Font12, WHITE, BLACK);      
            new_offset = y;      
        } 
    }
    return new_offset + 12; //new start y pos


}

void update_offservice(void){

    /*Drawing the current time*/
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    if (local_time != NULL) {
        char time_string[64];
        strftime(time_string, sizeof(time_string), "Updated:%H:%M:%S %d-%m-%Y", local_time);
        Paint_ClearWindows(EPD_2IN9_V2_HEIGHT - 140, 3, 150 + Font8.Width * 26, 3 + Font8.Height, WHITE);
        Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT - 140, 3, time_string, &Font8, WHITE, BLACK);
    } else {
        ESP_LOGE(TAG, "Failed to get local time");
    }   
    epaper_display_partial(BlackImage);
}

void display_bus(cJSON* response_1, cJSON* response_2){
    epaper_init_fast();
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);  	
    Paint_SelectImage(BlackImage);
    Paint_Clear(0xff);

    uint16_t idx = 0;
    idx = write_bus_stop(response_1,idx);
    idx = write_bus_stop(response_2,idx);    

    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    if (local_time != NULL) {
        char time_string[64];
        strftime(time_string, sizeof(time_string), "Updated:%H:%M:%S %d-%m-%Y", local_time);
        Paint_ClearWindows(EPD_2IN9_V2_HEIGHT - 140, 3, 150 + Font8.Width * 26, 3 + Font8.Height, WHITE);
        Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT - 140, 3, time_string, &Font8, WHITE, BLACK);
    } else {
        ESP_LOGE(TAG, "Failed to get local time");
    }  
    //epaper_display_partial(BlackImage);
    epaper_display(BlackImage);

    //epaper_overwrite_image(BlackImage); 
}
