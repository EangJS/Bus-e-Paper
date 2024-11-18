#include "epaper.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

UBYTE WS_20_30[159] =
{											
0x80,	0x66,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x40,	0x0,	0x0,	0x0,
0x10,	0x66,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x20,	0x0,	0x0,	0x0,
0x80,	0x66,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x40,	0x0,	0x0,	0x0,
0x10,	0x66,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x20,	0x0,	0x0,	0x0,
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x14,	0x8,	0x0,	0x0,	0x0,	0x0,	0x1,					
0xA,	0xA,	0x0,	0xA,	0xA,	0x0,	0x1,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x14,	0x8,	0x0,	0x1,	0x0,	0x0,	0x1,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x1,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x44,	0x44,	0x44,	0x44,	0x44,	0x44,	0x0,	0x0,	0x0,			
0x22,	0x17,	0x41,	0x0,	0x32,	0x36
};	

unsigned char Gray4[159] =			
{											
0x00,	0x60,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L0	 //2.28s			
0x20,	0x60,	0x10,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L1				
0x28,	0x60,	0x14,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L2				
0x2A,	0x60,	0x15,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L3 				
0x00,	0x90,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L4 				
0x00,	0x02,	0x00,	0x05,	0x14,	0x00,	0x00,						//TP, SR, RP of Group0				
0x1E,	0x1E,	0x00,	0x00,	0x00,	0x00,	0x01,						//TP, SR, RP of Group1				
0x00,	0x02,	0x00,	0x05,	0x14,	0x00,	0x00,						//TP, SR, RP of Group2				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group3				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group4				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group5				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group6				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group7				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group8				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group9				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group10				
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group11				
0x24,	0x22,	0x22,	0x22,	0x23,	0x32,	0x00,	0x00,	0x00,				//FR, XON				
0x22,	0x17,	0x41,	0xAE,	0x32,	0x28,							//EOPT VGH VSH1 VSH2 VSL VCOM				
};


extern spi_device_handle_t spi;

static void DEV_SPI_WriteByte(uint8_t data){
    spi_transaction_t transaction = {
    .length = 8,         // Data is 8 bits
    .tx_buffer = &data,
    };
    spi_device_transmit(spi, &transaction);
}

void epaper_send_command(UBYTE Reg)
{
    gpio_set_level(PIN_DC, 0);
    gpio_set_level(PIN_CS, 0);
    DEV_SPI_WriteByte(Reg);
    gpio_set_level(PIN_CS, 1);
}

void epaper_read_busy(void)
{
    UBYTE busy;
    do {
        epaper_send_command(0x71);
        busy = gpio_get_level(PIN_BUSY);
        if (busy == 0){
            break;
        }
        vTaskDelay(50);
    } while(busy);    
}

// Helper: Send Data to e-Paper
void epaper_send_data(uint8_t data) {
    gpio_set_level(PIN_DC, 1);
    gpio_set_level(PIN_CS, 0);
    spi_transaction_t transaction = {
        .length = 8,         // Data is 8 bits
        .tx_buffer = &data,
    };
    spi_device_transmit(spi, &transaction);
    gpio_set_level(PIN_CS, 1);
}

static void EPD_2IN9_V2_LUT(UBYTE *lut)
{
	UBYTE count;
	epaper_send_command(0x32);
	for(count=0; count<153; count++) 
		epaper_send_data(lut[count]); 
	epaper_read_busy();
}

static void EPD_2IN9_V2_LUT_by_host(UBYTE *lut)
{
	EPD_2IN9_V2_LUT((UBYTE *)lut);			//lut
	epaper_send_command(0x3f);
	epaper_send_data(*(lut+153));
	epaper_send_command(0x03);	// gate voltage
	epaper_send_data(*(lut+154));
	epaper_send_command(0x04);	// source voltage
	epaper_send_data(*(lut+155));	// VSH
	epaper_send_data(*(lut+156));	// VSH2
	epaper_send_data(*(lut+157));	// VSL
	epaper_send_command(0x2c);		// VCOM
	epaper_send_data(*(lut+158));
	
}



void epaper_reset(){
    gpio_set_level(PIN_RST, 1);
    vTaskDelay(10);
    gpio_set_level(PIN_RST, 0);
    vTaskDelay(2);
    gpio_set_level(PIN_RST, 1);
    vTaskDelay(10);
}

/******************************************************************************
function :	Setting the display window
parameter:
******************************************************************************/
static void epaper_set_windows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    epaper_send_command(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    epaper_send_data((Xstart>>3) & 0xFF);
    epaper_send_data((Xend>>3) & 0xFF);
	
    epaper_send_command(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    epaper_send_data(Ystart & 0xFF);
    epaper_send_data((Ystart >> 8) & 0xFF);
    epaper_send_data(Yend & 0xFF);
    epaper_send_data((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
******************************************************************************/
void epaper_set_cursor(UWORD Xstart, UWORD Ystart)
{
    epaper_send_command(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    epaper_send_data(Xstart & 0xFF);

    epaper_send_command(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    epaper_send_data(Ystart & 0xFF);
    epaper_send_data((Ystart >> 8) & 0xFF);
}

// Initialize e-Paper
void epaper_init() {
    // Reset the display
    epaper_reset();
    vTaskDelay(100);

    epaper_read_busy();
    epaper_send_command(0x12);
    epaper_read_busy();

    epaper_send_command(0x01);
    epaper_send_data(0x27);
	epaper_send_data(0x01);
	epaper_send_data(0x00);
	
	epaper_send_command(0x11); //data entry mode       
	epaper_send_data(0x03);
	
	epaper_set_windows(0, 0, EPD_2IN9_V2_WIDTH-1, EPD_2IN9_V2_HEIGHT-1);
	
	epaper_send_command(0x21); //  Display update control
	epaper_send_data(0x00);
	epaper_send_data(0x80);	
	
	epaper_set_cursor(0, 0);
	epaper_read_busy();
	
	EPD_2IN9_V2_LUT_by_host(WS_20_30);
 
}

void EPD_2IN9_V2_Gray4_Init(void)
{
	epaper_reset();
	vTaskDelay(100);

	epaper_read_busy();   
	epaper_send_command(0x12); // soft reset
	epaper_read_busy();
	
	epaper_send_command(0x01); //Driver output control      
	epaper_send_data(0x27);
	epaper_send_data(0x01);
	epaper_send_data(0x00);
	
	epaper_send_command(0x11); //data entry mode       
	epaper_send_data(0x03);
	
	epaper_set_windows(8, 0, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT-1);

	epaper_send_command(0x3C);       
	epaper_send_data(0x04);
	
	epaper_set_cursor(1, 0);
	epaper_read_busy();	

	EPD_2IN9_V2_LUT_by_host(Gray4);
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void epaper_turn_on_display(void)
{
	epaper_send_command(0x22); //Display Update Control
	epaper_send_data(0xc7);
	epaper_send_command(0x20); //Activate Display Update Sequence
	epaper_read_busy();
}


/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void epaper_clear_screen(void)
{
	UWORD i;
	
	epaper_send_command(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<4736;i++)
	{
        epaper_send_data(0xff);
	}
	epaper_turn_on_display();
}

void epaper_overwrite_image(UBYTE *Image)
{
    UDOUBLE i,j,k;
    UBYTE temp1,temp2,temp3;

    // old  data
    epaper_send_command(0x24);
    for(i=0; i<4736; i++) { 
        temp3=0;
        for(j=0; j<2; j++) {
            temp1 = Image[i*2+j];
            for(k=0; k<2; k++) {
                temp2 = temp1&0xC0;
                if(temp2 == 0xC0)
                    temp3 |= 0x00;
                else if(temp2 == 0x00)
                    temp3 |= 0x01; 
                else if(temp2 == 0x80)
                    temp3 |= 0x01; 
                else //0x40
                    temp3 |= 0x00; 
                temp3 <<= 1;

                temp1 <<= 2;
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0) 
                    temp3 |= 0x00;
                else if(temp2 == 0x00) 
                    temp3 |= 0x01;
                else if(temp2 == 0x80)
                    temp3 |= 0x01; 
                else    //0x40
                    temp3 |= 0x00;	
                if(j!=1 || k!=1)
                    temp3 <<= 1;

                temp1 <<= 2;
            }
        }
        epaper_send_data(temp3);
        // printf("%x ",temp3);
    }

    epaper_send_command(0x26);   //write RAM for black(0)/white (1)
    for(i=0; i<4736; i++) {            
        temp3=0;
        for(j=0; j<2; j++) {
            temp1 = Image[i*2+j];
            for(k=0; k<2; k++) {
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)
                    temp3 |= 0x00;//white
                else if(temp2 == 0x00)
                    temp3 |= 0x01;  //black
                else if(temp2 == 0x80)
                    temp3 |= 0x00;  //gray1
                else //0x40
                    temp3 |= 0x01; //gray2
                temp3 <<= 1;

                temp1 <<= 2;
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)  //white
                    temp3 |= 0x00;
                else if(temp2 == 0x00) //black
                    temp3 |= 0x01;
                else if(temp2 == 0x80)
                    temp3 |= 0x00; //gray1
                else    //0x40
                    temp3 |= 0x01;	//gray2
                if(j!=1 || k!=1)
                    temp3 <<= 1;

                temp1 <<= 2;
            }
        }
        epaper_send_data(temp3);
        // printf("%x ",temp3);
    }

    epaper_turn_on_display();
}

