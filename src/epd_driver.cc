#include "epd_driver.h"

using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::Null;

using v8::Local;
using v8::Value;
using v8::Function;  
using v8::FunctionTemplate;

#define DRIVER_OUTPUT_CMD		0x01
#define BOSTER_SOFT_START_CMD 	0x0c
#define DEEP_SLEEP_MODE_CMD		0x10
#define DATA_ENTRY_MODE_CMD		0x11
#define SWRESET_CMD				0x12
#define TEMPERATURE_SENSOR_CMD	0x1A
#define MASTER_ACTIVATION		0x20
#define DISPLAY_UPDATE_CONTROL	0x21
#define DISPLAY_UPDATE_CONTROL2	0x22
#define WRITE_RAM_CMD			0x24
#define WRITE_VCOM_REGISTER		0x2C
#define WRITE_LUT_REGISTER		0x32
#define SET_DUMMY_LINE_PERIOD	0x3A
#define SET_GATE_LINE_WIDTH		0x3B
#define BORDER_WAVEFORM_CONTROL	0x3C
#define SET_RAM_X_ADDRESS		0x44
#define SET_RAM_Y_ADDRESS		0x45
#define SET_RAM_X_COUNTER		0x4E
#define SET_RAM_Y_COUNTER		0x4F
#define NOP_CMD					0xFF

#define ENTRY_MODE_INCREMENT_Y_COUNTER (1<<2)
#define ENTRY_MODE_Y_INCREMENT (1<<1)
#define ENTRY_MODE_X_INCREMENT (1<<0)

unsigned char GDOControl[] = {DRIVER_OUTPUT_CMD, (yDot - 1) % 256, (yDot - 1) / 256, 0x00}; //for 1.54inch
unsigned char softstart[] = {BOSTER_SOFT_START_CMD, 0xd7, 0xd6, 0x9d};
unsigned char VCOMVol[] = {WRITE_VCOM_REGISTER, 0xa8};			 // VCOM 7c
unsigned char DummyLine[] = {SET_DUMMY_LINE_PERIOD, 0x1a};		 // 4 dummy line per gate
unsigned char Gatetime[] = {SET_GATE_LINE_WIDTH, 0x08};		 // 2us per line
unsigned char RamDataEntryMode[] = {DATA_ENTRY_MODE_CMD, ENTRY_MODE_X_INCREMENT}; // Ram data entry mode
//Write LUT register
#ifdef EPD02X13
unsigned char LUTDefault_full[] = {WRITE_LUT_REGISTER, 0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x01, 0x00, 0x00, 0x00, 0x00};
unsigned char LUTDefault_part[] = {WRITE_LUT_REGISTER, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#else
unsigned char LUTDefault_full[] = {WRITE_LUT_REGISTER, 0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51, 0x35, 0x51, 0x51, 0x19, 0x01, 0x00};
unsigned char LUTDefault_part[] = {WRITE_LUT_REGISTER, 0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

void SPI_Write(unsigned char value)
{
	int read_data;
	read_data = wiringPiSPIDataRW(0, &value, 1);
	if (read_data < 0)
		perror("wiringPiSPIDataRW failed\r\n");
}

void driver_delay_xms(unsigned long xms)
{
	delay(xms);
}
/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\						Drive layer								///|
			------------------------------------------------------------------------
***********************************************************************************************************************/
/*******************************************************************************
function：
			read busy
*******************************************************************************/
unsigned char ReadBusy(void)
{
	unsigned long i = 0;
	for (i = 0; i < 400; i++)
	{
		if (isEPD_BUSY == EPD_BUSY_LEVEL)
		{
			//printf("Busy is Low \r\n");
			return 1;
		}
		driver_delay_xms(10);
	}
	return 0;
}
/*******************************************************************************
function：
		write conmand
*******************************************************************************/
void EPD_WriteCMD(unsigned char command)
{
	EPD_DC_0; // command write
	SPI_Write(command);
}
/*******************************************************************************
function：
		write command and data
*******************************************************************************/
void EPD_WriteCMD_p1(unsigned char command, unsigned char para)
{
	ReadBusy();
	EPD_DC_0; // command write
	SPI_Write(command);
	EPD_DC_1; // command write
	SPI_Write(para);
}
/*******************************************************************************
function：
		Configure the power supply
*******************************************************************************/
void EPD_POWERON(void)
{
	EPD_WriteCMD_p1(DISPLAY_UPDATE_CONTROL2, 0xc0);
	EPD_WriteCMD(MASTER_ACTIVATION);
	//EPD_WriteCMD(0xff);
}

/*******************************************************************************
function：
		The first byte is written with the command value
		Remove the command value, 
		the address after a shift, 
		the length of less than one byte	
*******************************************************************************/
void EPD_Write(unsigned char *value, unsigned char datalen)
{
	unsigned char i = 0;
	unsigned char *ptemp;

	ptemp = value;
	//printf("send command  : 0x%02x \n",*ptemp);
	EPD_DC_0;		   // When DC is 0, write command
	SPI_Write(*ptemp); //The first byte is written with the command value
	ptemp++;
	EPD_DC_1; // When DC is 1, write data
	for (i = 0; i < datalen - 1; i++)
	{ // sub the data
		SPI_Write(*ptemp);
		ptemp++;
	}
}
/*******************************************************************************
Function: Write the display buffer
Parameters: 
		XSize x the direction of the direction of 128 points, adjusted to an 
		integer multiple of 8 times YSize y direction quantity Dispbuff displays 
		the data storage location. The data must be arranged in a correct manner
********************************************************************************/
void EPD_WriteDispRam(unsigned int XSize, unsigned int YSize,
					  unsigned char *Dispbuff)
{
	unsigned int i = 0, j = 0;
	if (XSize % 8 != 0)
	{
		XSize = XSize + (8 - XSize % 8);
	}
	XSize = XSize / 8;

	ReadBusy();
	EPD_DC_0; //command write
	SPI_Write(WRITE_RAM_CMD);
	EPD_DC_1; //data write
	for (i = 0; i < YSize; i++)
	{
		for (j = 0; j < XSize; j++)
		{
			SPI_Write(*Dispbuff);
			Dispbuff++;
		}
	}
}

/*******************************************************************************
Function: Write the display buffer to write a certain area to the same display.
Parameters: XSize x the direction of the direction of 128 points,adjusted to 
			an integer multiple of 8 times YSize y direction quantity  Dispdata 
			display data.
********************************************************************************/
void EPD_WriteDispRamMono(unsigned char XSize, unsigned int YSize,
						  unsigned char dispdata)
{
	unsigned int i = 0, j = 0;
	if (XSize % 8 != 0)
	{
		XSize = XSize + (8 - XSize % 8);
	}
	XSize = XSize / 8;

	ReadBusy();
	EPD_DC_0; // command write
	SPI_Write(WRITE_RAM_CMD);
	EPD_DC_1; // data write
	for (i = 0; i < YSize; i++)
	{
		for (j = 0; j < XSize; j++)
		{
			SPI_Write(dispdata);
		}
	}
}

/********************************************************************************
Set RAM X and Y -address Start / End position
********************************************************************************/
void EPD_SetRamArea(unsigned char Xstart, unsigned char Xend,
					unsigned char Ystart, unsigned char Ystart1, unsigned char Yend, unsigned char Yend1)
{
	unsigned char RamAreaX[3]; // X start and end
	unsigned char RamAreaY[5]; // Y start and end
	RamAreaX[0] = SET_RAM_X_ADDRESS;		   // command
	RamAreaX[1] = Xstart;
	RamAreaX[2] = Xend;
	RamAreaY[0] = SET_RAM_Y_ADDRESS; // command
	RamAreaY[1] = Ystart;
	RamAreaY[2] = Ystart1;
	RamAreaY[3] = Yend;
	RamAreaY[4] = Yend1;
	EPD_Write(RamAreaX, sizeof(RamAreaX));
	EPD_Write(RamAreaY, sizeof(RamAreaY));
}

/********************************************************************************
Set RAM X and Y -address counter
********************************************************************************/
void EPD_SetRamPointer(unsigned char addrX, unsigned char addrY, unsigned char addrY1)
{
	unsigned char RamPointerX[2]; // default (0,0)
	unsigned char RamPointerY[3];
	//Set RAM X address counter
	RamPointerX[0] = SET_RAM_X_COUNTER;
	RamPointerX[1] = addrX;
	//Set RAM Y address counter
	RamPointerY[0] = SET_RAM_Y_COUNTER;
	RamPointerY[1] = addrY;
	RamPointerY[2] = addrY1;

	EPD_Write(RamPointerX, sizeof(RamPointerX));
	EPD_Write(RamPointerY, sizeof(RamPointerY));
}

/********************************************************************************
1.Set RAM X and Y -address Start / End position
2.Set RAM X and Y -address counter
********************************************************************************/
void part_display(unsigned char RAM_XST, unsigned char RAM_XEND, unsigned char RAM_YST, unsigned char RAM_YST1, unsigned char RAM_YEND, unsigned char RAM_YEND1)
{
	EPD_SetRamArea(RAM_XST, RAM_XEND, RAM_YST, RAM_YST1, RAM_YEND, RAM_YEND1); /*set w h*/
	EPD_SetRamPointer(RAM_XST, RAM_YST, RAM_YST1);							   /*set orginal*/
}

//=========================functions============================
/*******************************************************************************
Initialize the register
********************************************************************************/
void EPD_Init(void)
{
	//1.reset driver
	EPD_RST_0; // Module reset
	driver_delay_xms(100);
	EPD_RST_1;
	driver_delay_xms(100);

	//2. set register
	EPD_Write(GDOControl, sizeof(GDOControl));											  // Pannel configuration, Gate selection
	EPD_Write(softstart, sizeof(softstart));											  // X decrease, Y decrease
	EPD_Write(VCOMVol, sizeof(VCOMVol));												  // VCOM setting
	EPD_Write(DummyLine, sizeof(DummyLine));											  // dummy line per gate
	EPD_Write(Gatetime, sizeof(Gatetime));												  // Gage time setting
	EPD_Write(RamDataEntryMode, sizeof(RamDataEntryMode));  // X increase, Y decrease
	EPD_SetRamArea(0x00, (xDot - 1) / 8, (yDot - 1) % 256, (yDot - 1) / 256, 0x00, 0x00); // X-source area,Y-gage area
	EPD_SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);						  // set ram
	EPD_WriteCMD_p1(DISPLAY_UPDATE_CONTROL, 1);	//disable bypass and GS0-GS4
}

/********************************************************************************
Display data updates
********************************************************************************/
void EPD_Update(void)
{
	EPD_WriteCMD_p1(DISPLAY_UPDATE_CONTROL2, 0xc7);
	EPD_WriteCMD(MASTER_ACTIVATION);
	EPD_WriteCMD(NOP_CMD);
}
void EPD_Update_Part(void)
{
	EPD_WriteCMD_p1(DISPLAY_UPDATE_CONTROL2, 0x04);
	EPD_WriteCMD(MASTER_ACTIVATION);
	EPD_WriteCMD(NOP_CMD);
}
/*******************************************************************************
write the waveform to the dirver's ram
********************************************************************************/
void EPD_WirteLUT(unsigned char *LUTvalue, unsigned char Size)
{
	EPD_Write(LUTvalue, Size);
}

/*******************************************************************************
Full screen initialization
********************************************************************************/
void EPD_init_Full(void)
{
	EPD_Init(); // Reset and set register
	EPD_WirteLUT((unsigned char *)LUTDefault_full, sizeof(LUTDefault_full));

	EPD_POWERON();
	//driver_delay_xms(100000);
}

/*******************************************************************************
Part screen initialization
********************************************************************************/
void EPD_init_Part(void)
{
	EPD_Init(); // display
	EPD_WirteLUT((unsigned char *)LUTDefault_part, sizeof(LUTDefault_part));
	EPD_POWERON();
}
/********************************************************************************
parameter:
	Label  :
       		=1 Displays the contents of the DisBuffer
	   		=0 Displays the contents of the first byte in DisBuffer,
********************************************************************************/
void EPD_Dis_Full(unsigned char *DisBuffer, unsigned char Label)
{
	EPD_SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256); // set ram
	if (Label == 0)
	{
		EPD_WriteDispRamMono(xDot, yDot, 0xff); // white
	}
	else
	{
		EPD_WriteDispRam(xDot, yDot, (unsigned char *)DisBuffer); // white
	}
	EPD_Update();
}

/********************************************************************************
parameter: 
		xStart :   X direction Start coordinates
		xEnd   :   X direction end coordinates
		yStart :   Y direction Start coordinates
		yEnd   :   Y direction end coordinates
		DisBuffer : Display content
		Label  :
       		=1 Displays the contents of the DisBuffer
	   		=0 Displays the contents of the first byte in DisBuffer,
********************************************************************************/
void EPD_Dis_Part(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd, unsigned char *DisBuffer, unsigned char Label)
{
	//printf("EDP_Dis_Part x=%d-%d y=%d-%d l=%d\r\n", xStart, xEnd, yStart, yEnd, Label);
	if (Label == 0)
	{ // black
		part_display(xStart / 8, xEnd / 8, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
		EPD_WriteDispRamMono(xEnd - xStart, yEnd - yStart + 1, DisBuffer[0]);
		EPD_Update_Part();
		driver_delay_xms(500);
		part_display(xStart / 8, xEnd / 8, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
		EPD_WriteDispRamMono(xEnd - xStart, yEnd - yStart + 1, DisBuffer[0]);
	}
	else
	{ // show
		part_display(xStart / 8, xEnd / 8, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
		EPD_WriteDispRam(xEnd - xStart, yEnd - yStart + 1, DisBuffer);
		EPD_Update_Part();
		//driver_delay_xms(100);
		part_display(xStart / 8, xEnd / 8, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
		EPD_WriteDispRam(xEnd - xStart, yEnd - yStart + 1, DisBuffer);
	}
}

/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\						App layer								///|
			------------------------------------------------------------------------
***********************************************************************************************************************/
/********************************************************************************
		clear full screen
********************************************************************************/
void Dis_Clear_full(void)
{
	unsigned char m;
	//init
	//printf("1.init full screen\r\n");
	EPD_init_Full();
	driver_delay_xms(DELAYTIME);

	//Clear screen
	m = 0xff;
	//printf("2.Clear screen\r\n");
	EPD_Dis_Full((unsigned char *)&m, 0); //all white
	driver_delay_xms(DELAYTIME);
}

/********************************************************************************
		clear part screen
********************************************************************************/
void Dis_Clear_part(void)
{
	unsigned char m;
	//init
	//printf("1.init part screen\r\n");
	EPD_init_Part();
	driver_delay_xms(DELAYTIME);

	//Clear screen
	//printf("2.Clear screen\r\n");
	m = 0xff;
	EPD_Dis_Part(0, xDot - 1, 0, yDot - 1, (unsigned char *)&m, 0); //all white
	driver_delay_xms(DELAYTIME);
}

/********************************************************************************
parameter: 
		xStart :   X direction Start coordinates
		xEnd   :   X direction end coordinates
		yStart :   Y direction Start coordinates
		yEnd   :   Y direction end coordinates
		DisBuffer : Display content
********************************************************************************/
void Dis_full_pic(unsigned char *DisBuffer)
{
	EPD_Dis_Full((unsigned char *)DisBuffer, 1);
}

void Dis_part_pic(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd, unsigned char *DisBuffer)
{
	EPD_Dis_Part(xStart, xEnd, yStart, yEnd, (unsigned char *)DisBuffer, 1);
}

void Dis_Drawing(unsigned int xStart,unsigned int yStart,unsigned char *DisBuffer,unsigned int xSize,unsigned int ySize)
{
	unsigned int x_addr = xStart*8;
	unsigned int y_addr = yStart*8;
	EPD_Dis_Part(y_addr,y_addr+xSize-1,yDot-ySize-x_addr,yDot-x_addr-1,(unsigned char *)DisBuffer,1);
}
