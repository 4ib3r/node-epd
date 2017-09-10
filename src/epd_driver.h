#ifndef EPD_DRIVER_H
#define EPD_DRIVER_H

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <nan.h>
#include <v8.h>


#define EPD2X9 1
#ifdef EPD2X9
	#define LCDWIDTH 296
	#define LCDHEIGHT 128
    #define DELAYTIME 1500
    #define DPI 112 //13064
#elif  EPD02X13
	#define LCDWIDTH 250
	#define LCDHEIGHT 122
	#define DELAYTIME 4000
    #define DPI 131 //14319
#elif  EPD1X54
	#define LCDWIDTH 200
	#define LCDHEIGHT 200
	#define DELAYTIME 1500
    #define DPI 184 //25974
#endif

#define xDot LCDHEIGHT
#define yDot LCDWIDTH

//GPIO config
#define RST 17
#define DC  25
#define BUSY 24
#define CS 8	//CE0

#define EPD_RST_0	digitalWrite(RST,LOW)
#define EPD_RST_1	digitalWrite(RST,HIGH)
#define isEPD_RST  digitalRead(RST)

#define EPD_DC_1	digitalWrite(DC,HIGH)
#define EPD_DC_0	digitalWrite(DC,LOW)
#define isEPD_DC  digitalRead(DC)

#define EPD_BUSY_LEVEL 0
#define isEPD_BUSY  digitalRead(BUSY)

void driver_delay_xms(unsigned long xms);
void EPD_Init(void);
void Dis_Clear_full(void);
void Dis_Clear_part(void);
void EPD_init_Part(void);
void EPD_init_Full(void);
void Dis_full_pic(unsigned char *DisBuffer);
void EPD_Dis_Full(unsigned char *DisBuffer, unsigned char Label);
void EPD_Dis_Part(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd, unsigned char *DisBuffer, unsigned char Label);
void Dis_part_pic(unsigned int xStart,unsigned int xEnd,unsigned int yStart,unsigned int yEnd,unsigned char *DisBuffer);
void Dis_Drawing(unsigned int xStart,unsigned int yStart,unsigned char *DisBuffer,unsigned int xSize,unsigned int ySize);
void EPD_WriteDispRamMono(unsigned char XSize, unsigned int YSize,
	unsigned char dispdata);
void EPD_Update(void);



#endif