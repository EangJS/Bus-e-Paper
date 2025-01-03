#include "bus.h"

#define TAG "BUS"

static char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            } else{
                // Process chunked data (each chunk received in evt->data)
                ESP_LOGD(TAG, "Chunked data received: %.*s", evt->data_len, (char *)evt->data);
                // Optionally, process or accumulate chunked data here
                if (evt->user_data) {
                    // Accumulate data into user-provided buffer
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    // Dynamically allocate/expand buffer for chunked data
                    if (!output_buffer) {
                        output_buffer = (char *) malloc(evt->data_len);
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                        output_len = 0;
                    } else {
                        char *new_buffer = realloc(output_buffer, output_len + evt->data_len);
                        if (new_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to reallocate memory for chunked data");
                            free(output_buffer);
                            output_buffer = NULL;
                            return ESP_FAIL;
                        }
                        output_buffer = new_buffer;
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len; 
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            break;
    }
    return ESP_OK;
}

cJSON* get_bus_timing(char* bus_stop_code){
    char full_url[256];
    snprintf(full_url, sizeof(full_url), "%s?BusStopCode=%s", BASE_URL, bus_stop_code);
    //ESP_LOGI(TAG, "%s", full_url);

    esp_http_client_config_t config = {
        .url=full_url,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        .disable_auto_redirect = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "AccountKey", API_KEY);
        // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        long long content_length = esp_http_client_get_content_length(client);

        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                status_code,
                content_length);
                
        cJSON *json = cJSON_Parse(local_response_buffer);
        
        if (json == NULL) {
           ESP_LOGE(TAG, "Error parsing JSON response.");
           esp_http_client_cleanup(client);
           return NULL;
        }
        esp_http_client_cleanup(client);
        return json;

    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return NULL;
}
