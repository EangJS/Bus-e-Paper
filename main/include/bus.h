#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
#include "secrets.h"

#define MAX_HTTP_OUTPUT_BUFFER 4096

cJSON* get_bus_timing(char* bus_stop_code);
