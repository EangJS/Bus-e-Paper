/**
 * @file wifi.h
 * @author Eugene Ang
 * @brief Connect the ESP32 to Wi-Fi
 * @version 1.1.0
 * @date 2024-11-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "secrets.h"

/**
 * @brief Performs the connection to Wi-Fi for ESP32's Internet access
 * 
 */
void connect_to_wifi(void);
