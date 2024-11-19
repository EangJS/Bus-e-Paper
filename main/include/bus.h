/**
 * @file bus.h
 * @author Eugene Ang
 * @brief Header file for the bus timing functions
 * @version 1.0.0
 * @date 2024-11-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
#include "secrets.h"

#define MAX_HTTP_OUTPUT_BUFFER 4096

/**
 * @brief Get the bus timing information from the LTA API
 * 
 * @param bus_stop_code The bus stop code to query
 * @return cJSON* JSON response from the API
 */
cJSON* get_bus_timing(char* bus_stop_code);
