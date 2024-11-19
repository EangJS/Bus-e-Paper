/**
 * @file draw.h
 * @author Eugene Ang
 * @brief Header file for draw.c - Drawing functions for the bus stop data
 * @version 1.1.0
 * @date 2024-11-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "paint.h"
#include <stdlib.h> 
#include "Fonts/fonts.h"
#include "cJSON.h"
#include "esp_log.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Display the bus timings on the e-paper display for
 * two bus stops.
 * 
 * @param response_1 Response from the first bus stop
 * @param response_2 Response from the second bus stop
 */
void display_bus(cJSON* response_1, cJSON* response_2);

/**
 * @brief Update the e-paper display for off-service hours
 * 
 */
void update_offservice(void);
