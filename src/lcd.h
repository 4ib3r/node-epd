#ifndef PCDLCD_H
#define PCDLCD_H

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <ft2build.h>

#include <v8.h>
#include <node.h>
#include <nan.h>
#include "epd_driver.h"
#include "FontFace.h"

using namespace v8;
using namespace node;

#include FT_GLYPH_H


#define swap_bits(a, b) { uint8_t t = a; a = b; b = t; }

 // calibrate clock constants
#define CLKCONST_1  8000
#define CLKCONST_2  400  // 400 is a good tested value for Raspberry Pi

// keywords
#define LSBFIRST  0
#define MSBFIRST  1


class EpdLcd : public Nan::ObjectWrap {
    public:
        static void Init();
        static v8::Local<v8::Object> NewInstance(v8::Local<v8::Value> channel, v8::Local<v8::Value> dc, v8::Local<v8::Value> cs, v8::Local<v8::Value> rst, v8::Local<v8::Value> led);
        static NAN_METHOD(New);
        static NAN_METHOD(Contrast);
        static NAN_METHOD(Backlight);
        static NAN_METHOD(ShowLogo);
        static NAN_METHOD(Size);
        static NAN_METHOD(Data);
        static NAN_METHOD(Clear);
        static NAN_METHOD(ClearPart);
        static NAN_METHOD(Update);
        static NAN_METHOD(UpdateAsync);
        static NAN_METHOD(BlitPart);
        static NAN_METHOD(Color);
        static NAN_METHOD(Line);
        static NAN_METHOD(Rect);
        static NAN_METHOD(Pixel);
        static NAN_METHOD(Circle);
        static NAN_METHOD(Font);
        static NAN_METHOD(DrawChar);
        static NAN_METHOD(DrawString);
        static NAN_METHOD(Image);
    private:
        EpdLcd(uint8_t channel, uint8_t dc, uint8_t cs, uint8_t rst, uint8_t led );
        ~EpdLcd();

        static Nan::Persistent<v8::Function> constructor;
        uint8_t channel;
        uint8_t _led;
        char *buff;
        long int screenSize = LCDWIDTH * LCDHEIGHT;

        uint8_t color = 1;

        const char *fontName;
        uint8_t fontSize;
        bool fontBold;


        bool drawToBuffer;
        FT_Matrix     matrix;
	    FT_Library         library;
	    FT_Face            face;
	    FT_BitmapGlyphRec *g;
};

class DisUpdateFullWorker : public Nan::AsyncWorker {
public:
  DisUpdateFullWorker(Nan::Callback *callback) : AsyncWorker(callback) {};
  ~DisUpdateFullWorker() {};
  void Execute();
protected:  
  void HandleOKCallback();
};


#endif
