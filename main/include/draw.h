#include "paint.h"
#include <stdlib.h> 
#include "Fonts/fonts.h"
#include "cJSON.h"
#include "esp_log.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

void display_bus(cJSON* response_1, cJSON* response_2);
void update_offservice(void);
