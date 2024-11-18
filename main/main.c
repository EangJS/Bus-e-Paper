/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "epaper.h"
#include "draw.h"
#include "esp_http_client.h"
#include "bus.h"
#include "wifi.h"
#include "cJSON.h"
#include <sys/time.h>
#include "esp_sntp.h"

static const char *TAG = "MAIN";

// SPI Clock Speed
#define SPI_CLOCK_SPEED_HZ  4000000 // 4 MHz (Adjust as needed)


spi_device_handle_t spi;


void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG,"Time synchronized\n");
}

void initialize_sntp() {
    printf("Initializing SNTP...\n");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org"); // Use a default NTP server
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();
    setenv("TZ", "UTC-8", 1);
    tzset();
}

// Task to control e-Paper
void epaper_task(void *arg) {
    ESP_LOGI(TAG,"Resetting Display");

    epaper_init();
    epaper_clear_screen();
    //draw_sample();

    int active = 1;
    
    while (1) {
        cJSON* response_1 = get_bus_timing("55029");
        cJSON* response_2 = get_bus_timing("55021");

        if (response_1 == NULL || response_2 == NULL) {
            ESP_LOGE(TAG, "Error fetching bus timings");
            vTaskDelay(60000 / portTICK_PERIOD_MS); // Retry after 1 minute
            continue;
        }

        cJSON* services_1 = cJSON_GetObjectItem(response_1, "Services");
        cJSON* services_2 = cJSON_GetObjectItem(response_2, "Services");
        uint8_t num_services_1 = cJSON_GetArraySize(services_1);
        uint8_t num_services_2 = cJSON_GetArraySize(services_2);
        if(num_services_1 > 0 && num_services_2 > 0){
            display_bus(response_1,response_2); // there are services available to show
            active = 1;
        } else{            
            if (active) display_bus(response_1, response_2);
            active = 0;   
            ESP_LOGI("SLEEP", "Off-Service - Extra minute sleep");  
            vTaskDelay(60000 / portTICK_PERIOD_MS); // Inactive, update additional minute       
        }
        cJSON_Delete(response_1);
        cJSON_Delete(response_2);
          
        vTaskDelay(60000 / portTICK_PERIOD_MS); // Update every minute
    }
    
}

// Main Application
void app_main(void) {
    // Configure GPIO pins
    gpio_set_direction(PIN_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_BUSY, GPIO_MODE_INPUT);

    //startup wifi
    connect_to_wifi();
    ESP_LOGI(TAG,"Connected to WiFi");

    initialize_sntp();

    // SPI Configuration
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_MOSI,
        .sclk_io_num = PIN_SCLK,
        .miso_io_num = -1, // Not used
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_CLOCK_SPEED_HZ,
        .mode = 0,                        // SPI mode 0
        .spics_io_num = PIN_CS,
        .queue_size = 7,
    };

    // Initialize SPI Bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));

    // Start e-Paper Task
    xTaskCreate(epaper_task, "epaper_task", 8192, NULL, 5, NULL);
}
