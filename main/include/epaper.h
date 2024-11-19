/**
 * @file epaper.h
 * @author Eugene Ang
 * @brief Header file for the e-paper display - Handles the initialization and display of the e-paper display
 * @version 1.1.0
 * @date 2024-11-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <inttypes.h>

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define EPD_2IN9_V2_WIDTH       128
#define EPD_2IN9_V2_HEIGHT      296

// Display Commands (Example commands for Waveshare 2.9 e-Paper)
#define CMD_RESET_DISPLAY   0x12
#define CMD_DRIVER_OUTPUT   0x01

#define PIN_MOSI    23
#define PIN_SCLK    18
#define PIN_CS      19
#define PIN_DC      21
#define PIN_RST     22
#define PIN_BUSY    5

/**
 * @brief Initialize the e-paper display
 * 
 */
void epaper_init(void);

/**
 * @brief Clear the e-paper display
 * 
 */
void epaper_clear_screen(void);

/**
 * @brief Overwrite the image on the e-paper display
 * 
 * @param Image Image to be displayed
 */
void epaper_overwrite_image(UBYTE *Image);

/**
 * @brief Update the e-paper display with a partial update
 * 
 * @param Image Image to be displayed
 */
void epaper_display_partial(UBYTE *Image);

/**
 * @brief Initialize the e-paper display with fast initialization (two flashes)
 * 
 */
void epaper_init_fast(void);

/**
 * @brief Display the image on the e-paper display (full update)
 * 
 * @param Image Image to be displayed
 */
void epaper_display(UBYTE *Image);
