#include <inttypes.h>

/**
 * data
**/
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

// Function prototypes
void epaper_init(void);
void epaper_reset(void);
void epaper_clear_screen(void);
void epaper_set_cursor(UWORD Xstart, UWORD Ystart);
void epaper_read_busy(void);
void epaper_send_command(UBYTE command);
void epaper_send_data(uint8_t data);
void epaper_overwrite_image(UBYTE *Image);
void EPD_2IN9_V2_Gray4_Init(void);
void epaper_display_partial(UBYTE *Image);
void epaper_init_fast(void);
void epaper_display(UBYTE *Image);
