# E-Paper Bus Arrival Display

A low power IOT device to show you bus arrival timings so you know when to leave your house. Powered by the ESP-32-DEVKITV1 and WaveShare 2.9 inch e-Paper V2.

Shows the estimated arrival time and type of bus. DD - Double Deck | SD - Single Deck of up to 2 bus stops with maximum of 8-9 services.

![1731932145950](https://file+.vscode-resource.vscode-cdn.net/c%3A/Users/eugen/Documents/Workspace/Waveshare/image/README/1731932145950.png "Bus Display")

## Getting Started

⚠️ Note: You must first obtain your own api key from [LTA DataMall](https://datamall.lta.gov.sg/content/datamall/en/request-for-api.html).

1. Clone this repository.
2. Include a `secrets.h` file in the `/main/include` directory with the following contents.

   ```
   #define WIFI_SSID "<YOUR WI-FI SSID>" 
   #define WIFI_PASS "<YOUR WI-FI PASSWORD>" 
   #define BASE_URL "http://datamall2.mytransport.sg/ltaodataservice/BusArrivalv2"  // The base URL without parameters
   #define API_KEY "<YOUR API KEY>"
   ```
3. Compile and build with the [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html).
4. Connect the wirings to the e-Paper display accordingly.

| E-Paper | ESP32 DEVKITV1 |
| ------- | -------------- |
| DIN     | 23             |
| CLK     | 18             |
| CS      | 19             |
| DC      | 21             |
| RST     | 22             |
| BUSY    | 5              |
| VCC     | 3.3V           |
| GND     | GND            |

5. Power on and display it.

## Configuring Bus Stops

You can configure the bus stops you want to display in `main.c`.

Simply edit the lines by changing the bus stop code to your desired ones.

```
        cJSON* response_1 = get_bus_timing("55029");
        cJSON* response_2 = get_bus_timing("55021");
```

⚠️ Note: The firmware currently only supports 2 bus stops due to display limitations.

🔨 Work in Progress: Web-UI to edit WiFi SSID and Password as well as configure bus stop.

You can also configure the referesh rate. Default is set to 1 minute for operational periods and 2 minutes for non-operational periods.

Simply edit the lines in `main.c` by changing the value 60000 (in ms).

```
vTaskDelay(60000 / portTICK_PERIOD_MS); // Update every minute
```

🔨 Work in Progress: Web-UI to edit refresh rate.

## Troubleshooting

* Program upload failure

  * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
  * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

## Credits

* [ESP API Guides](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/index.html)
* [waveshareteam/e-paper](https://github.com/waveshareteam/e-Paper/tree/master/Arduino_R4/src)
* [LTA DataMall API](https://datamall.lta.gov.sg/content/datamall/en.html)
* ChatGPT generated content
