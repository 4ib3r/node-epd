#include "lcd.h"
using namespace v8;
using Nan::Callback;

// An abs() :)
#define abs(a) (((a) < 0) ? -(a) : (a))
#define max(a,b) ((a) < (b) ? (b) : (a))
#define ceilByte(a) (((a + 8 - 1) / 8)*8);
#define floorByte(a) ((a / 8)*8);

// bit set
#define _BV(bit) (1 << (bit))

Nan::Persistent<Function> EpdLcd::constructor;

#define enablePartialUpdate

static unsigned char lcd_buff[LCDHEIGHT * LCDWIDTH * 8];

#ifdef enablePartialUpdate
static uint16_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
static bool changed = false;
#endif

void updateBoundingBox(uint16_t xmin, uint16_t ymin, uint16_t xmax, uint16_t ymax) {
#ifdef enablePartialUpdate
    changed = true;
	if (xmin < xUpdateMin) xUpdateMin = xmin;
	if (xmax > xUpdateMax) xUpdateMax = xmax;
	if (ymin < yUpdateMin) yUpdateMin = ymin;
	if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}

void EpdLcd::Init()
{
    Nan::HandleScope scope;

    Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(EpdLcd::New);
    ctor->InstanceTemplate()->SetInternalFieldCount(1);
    ctor->SetClassName(Nan::New("EpdLcd").ToLocalChecked());

    ctor->PrototypeTemplate()->Set(Nan::New("size").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Size));
    ctor->PrototypeTemplate()->Set(Nan::New("backlight").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Backlight));
    ctor->PrototypeTemplate()->Set(Nan::New("data").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Data));
    ctor->PrototypeTemplate()->Set(Nan::New("clear").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Clear));
    ctor->PrototypeTemplate()->Set(Nan::New("partClear").ToLocalChecked(),
                                    Nan::New<FunctionTemplate>(ClearPart));
    ctor->PrototypeTemplate()->Set(Nan::New("update").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Update));
    ctor->PrototypeTemplate()->Set(Nan::New("partUpdate").ToLocalChecked(),
                                    Nan::New<FunctionTemplate>(UpdatePart));
    ctor->PrototypeTemplate()->Set(Nan::New("color").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Color));
    ctor->PrototypeTemplate()->Set(Nan::New("line").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Line));
    ctor->PrototypeTemplate()->Set(Nan::New("rect").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Rect));
    ctor->PrototypeTemplate()->Set(Nan::New("pixel").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Pixel));
    ctor->PrototypeTemplate()->Set(Nan::New("circle").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Circle));
    ctor->PrototypeTemplate()->Set(Nan::New("font").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Font));
    ctor->PrototypeTemplate()->Set(Nan::New("drawChar").ToLocalChecked(),
                                    Nan::New<FunctionTemplate>(DrawChar));
    ctor->PrototypeTemplate()->Set(Nan::New("drawString").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(DrawString));
    ctor->PrototypeTemplate()->Set(Nan::New("image").ToLocalChecked(),
                                   Nan::New<FunctionTemplate>(Image));

    constructor.Reset(ctor->GetFunction());
}

static unsigned int pointerPos(unsigned int x, unsigned int y) {
    return x * (LCDHEIGHT / 8) + ((y / 8) % 128);
}

void EpdSetPixel(unsigned int x, unsigned int y, short color)
{
    unsigned int pos = pointerPos(x,y);
    if (color < 0) 
    {
		lcd_buff[pos] ^= _BV(7 - (y % 8));
    } 
    else if (color == 0)
    {
        lcd_buff[pos] |= _BV(7 - (y % 8));
    }
    else
    {
        lcd_buff[pos] &= ~_BV(7 - (y % 8));
    }
}

Local<Object> EpdLcd::NewInstance(Local<Value> channel, Local<Value> dc, Local<Value> cs, Local<Value> rst, Local<Value> led)
{

    Nan::EscapableHandleScope scope;

    const unsigned argc = 5;
    Local<Value> argv[argc] = {channel, dc, cs, rst, led};
    Local<Function> cons = Nan::New<Function>(constructor);
    Local<Object> instance = Nan::NewInstance(cons, argc, argv).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(EpdLcd::New)
{
    Nan::HandleScope scope;

    uint8_t led = info[1]->IsUndefined() ? 127 : info[0]->NumberValue();
    uint8_t ch = info[1]->IsUndefined() ? 0 : info[1]->NumberValue();
    uint8_t dc = info[2]->IsUndefined() ? 25 : info[2]->NumberValue();
    uint8_t cs = info[3]->IsUndefined() ? 8 : info[3]->NumberValue();
    uint8_t rst = info[4]->IsUndefined() ? 17 : info[4]->NumberValue();

    EpdLcd *obj = new EpdLcd(ch, dc, cs, rst, led);
    obj->_led = led;
    //obj->drawToBuffer = info[1]->IsUndefined() ? false : info[1]->BooleanValue();

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(EpdLcd::Size)
{
    Nan::HandleScope scope;
    Local<Object> sizeObject = Nan::New<Object>();
    sizeObject->Set(Nan::New<String>("width").ToLocalChecked(), Nan::New<Number>(LCDWIDTH));
    sizeObject->Set(Nan::New<String>("height").ToLocalChecked(), Nan::New<Number>(LCDHEIGHT));
    info.GetReturnValue().Set(sizeObject);
}

NAN_METHOD(EpdLcd::Backlight)
{
    Nan::HandleScope scope;
    EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());
    if (!info[0]->IsUndefined())
    {
        uint8_t val = info[0]->NumberValue();
        pwmWrite(obj->_led, val);
    }
    /*if (!info[0]->IsUndefined() && info[0]->BooleanValue()) {
      digitalWrite(obj->_led, HIGH);
    } else {
      digitalWrite(obj->_led, LOW);      
    }*/
    return;
}

NAN_METHOD(EpdLcd::Data)
{
    Nan::HandleScope scope;
    EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());
    Local<Object> bufferObject = Nan::NewBuffer(obj->buff, obj->screenSize * 8).ToLocalChecked();
    uint16_t i;
    for (i = 0; i < obj->screenSize * 8; i++)
    {
        obj->buff[i] = lcd_buff[i];
    }
    info.GetReturnValue().Set(bufferObject);
}

NAN_METHOD(EpdLcd::Clear)
{
    Nan::HandleScope scope;
    if (info[0]->IsUndefined()) {
        //LCDclear();
        for(uint16_t i = 0; i < LCDWIDTH * LCDHEIGHT / 8; i++) {
            lcd_buff[i] = 0xff;
        }
        Dis_Clear_full();
    } else {
	  Callback *callback = new Callback(info[0].As<Function>());
      AsyncQueueWorker(new DisClearFullWorker(callback));
    }
    return;
}

NAN_METHOD(EpdLcd::ClearPart)
{
    Nan::HandleScope scope;
    //LCDclear();
    Dis_Clear_part();
    return;
}

NAN_METHOD(EpdLcd::Update)
{
    Nan::HandleScope scope;
    #ifdef enablePartialUpdate
        changed = false;
        xUpdateMin = LCDWIDTH - 1;
        xUpdateMax = 0;
        yUpdateMin = LCDHEIGHT-1;
        yUpdateMax = 0;
    #endif
    if (info[0]->IsUndefined()) {
        Dis_full_pic((unsigned char *)lcd_buff);
    } else {
        Callback *callback = new Callback(info[0].As<Function>());
        AsyncQueueWorker(new DisUpdateFullWorker(callback));
    }
    return;
}

void DisClearFullWorker::Execute()
{
    //LCDclear();
    for(uint16_t i = 0; i < LCDWIDTH * LCDHEIGHT / 8; i++) {
        lcd_buff[i] = 0xff;
    }
	Dis_Clear_full();
};
void DisClearFullWorker::HandleOKCallback()
{
	Nan::HandleScope scope;

	Local<Value> argv[] = {
		Nan::Null()};

	callback->Call(2, argv);
};

void DisUpdateFullWorker::Execute()
{
    #ifdef enablePartialUpdate
    changed = false;
    xUpdateMin = LCDWIDTH - 1;
    xUpdateMax = 0;
    yUpdateMin = LCDHEIGHT-1;
    yUpdateMax = 0;
    #endif
    Dis_full_pic((unsigned char *)lcd_buff);
    driver_delay_xms(DELAYTIME); 
};

void DisUpdateFullWorker::HandleOKCallback()
{
	Nan::HandleScope scope;
	Local<Value> argv[] = {
        Nan::Null()
    };

	callback->Call(2, argv);
}

bool updateScreenPart() {
    if (!changed) {
         return false;
    }
    uint16_t x = floorByte(xUpdateMin);
    uint16_t y = floorByte(yUpdateMin);
    uint16_t x2 = xUpdateMax;
    uint16_t y2 = ceilByte(yUpdateMax);
    uint16_t w = x2 - x;
    uint16_t h = y2 - y;

    uint16_t pos_s = pointerPos(x, y);
    uint16_t len = ((w+1)*h/8);
    unsigned char buff_part[len] = { 0xff };
    uint16_t j = 0;
    for (uint16_t i = 0; i < w; i++)
    {
        //Copy part for next lines
        memcpy(buff_part + j, lcd_buff + (pos_s + (i*LCDHEIGHT/8)), h/8);
        j += h/8;
    }
    Dis_Drawing(x/8, y/8, (unsigned char *)buff_part, h, w);
    #ifdef enablePartialUpdate
        changed = false;
        xUpdateMin = LCDWIDTH - 1;
        xUpdateMax = 0;
        yUpdateMin = LCDHEIGHT-1;
        yUpdateMax = 0;
    #endif
    return true;
}

NAN_METHOD(EpdLcd::UpdatePart)
{
    Nan::HandleScope scope;
    if (info[0]->IsUndefined()) {
        bool res = updateScreenPart();
        info.GetReturnValue().Set(Nan::New(res));
    } else {
        Callback *callback = new Callback(info[0].As<Function>());
        AsyncQueueWorker(new DisUpdatePartWorker(callback));
    }
    return;
}

void DisUpdatePartWorker::Execute()
{
    this->res = updateScreenPart();
};

void DisUpdatePartWorker::HandleOKCallback()
{
	Nan::HandleScope scope;
	Local<Value> argv[] = {
        Nan::New(this->res),
    };

	callback->Call(1, argv);
}

NAN_METHOD(EpdLcd::Color)
{
    Nan::HandleScope scope;

    EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());
    obj->color = (info[0]->NumberValue());
    return;
}

// bresenham's algorithm - thx wikpedia
void LCDdrawline(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color)
{
	uint16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		swap_bits(x0, y0);
		swap_bits(x1, y1);
	}

	if (x0 > x1)
	{
		swap_bits(x0, x1);
		swap_bits(y0, y1);
	}

	uint16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1)
	{
		ystep = 1;
	} else
	{
		ystep = -1;
	}

	for (; x0<=x1; x0++)
	{
		if (steep)
		{
			EpdSetPixel(y0, x0, color);
		}
		else
		{
			EpdSetPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

NAN_METHOD(EpdLcd::Line)
{
    Nan::HandleScope scope;

    unsigned int x0 = (info[0]->NumberValue());
    unsigned int y0 = (info[1]->NumberValue());
    unsigned int x1 = (info[2]->NumberValue());
    unsigned int y1 = (info[3]->NumberValue());

    EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());
    LCDdrawline(x0, y0, x1, y1, obj->color);

    return;
}


NAN_METHOD(EpdLcd::Rect)
{
    Nan::HandleScope scope;

    unsigned int x = (info[0]->NumberValue());
    unsigned int y = (info[1]->NumberValue());
    unsigned int w = (info[2]->NumberValue());
    unsigned int h = (info[3]->NumberValue());

    EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());
    uint8_t col = obj->color;
    if (!info[5]->IsUndefined()) col = info[5]->NumberValue();

    if (!info[4]->IsUndefined() && info[4]->BooleanValue() == true)
    {
        //    LCDfillrect(x, y, w, h, obj->color);
        uint16_t i,j;
        for ( i=x; i<x+w; i++)
        {
            for ( j=y; j<y+h; j++)
            {
                EpdSetPixel(i, j, col);
            }
        }
        updateBoundingBox(x, y, x+w, y+h);
    }
    else
    {
        uint16_t i;
        for ( i=x; i<x+w; i++) {
            EpdSetPixel(i, y, obj->color);
            EpdSetPixel(i, y+h-1, obj->color);
        }
        for ( i=y; i<y+h; i++) {
            EpdSetPixel(x, i, obj->color);
            EpdSetPixel(x+w-1, i, obj->color);
        }
        updateBoundingBox(x, y, x+w, y+h);
        //    LCDdrawrect(x, y, w, h, obj->color);
    }
    return;
}

NAN_METHOD(EpdLcd::Pixel)
{
    Nan::HandleScope scope;

    unsigned int x = (info[0]->NumberValue());
    unsigned int y = (info[1]->NumberValue());
    bool color = (info[2]->BooleanValue());

    //EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());

    EpdSetPixel(x, y, color);
    return;
}



// draw a circle outline
void LCDdrawcircle(uint16_t x0, uint16_t y0, uint16_t r, uint8_t color)
{
	updateBoundingBox(x0-r, y0-r, x0+r, y0+r);

    int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	EpdSetPixel(x0, y0+r, color);
	EpdSetPixel(x0, y0-r, color);
	EpdSetPixel(x0+r, y0, color);
	EpdSetPixel(x0-r, y0, color);

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		EpdSetPixel(x0 + x, y0 + y, color);
		EpdSetPixel(x0 - x, y0 + y, color);
		EpdSetPixel(x0 + x, y0 - y, color);
		EpdSetPixel(x0 - x, y0 - y, color);

		EpdSetPixel(x0 + y, y0 + x, color);
		EpdSetPixel(x0 - y, y0 + x, color);
		EpdSetPixel(x0 + y, y0 - x, color);
		EpdSetPixel(x0 - y, y0 - x, color);

	}
}

void LCDfillcircle(uint16_t x0, uint16_t y0, uint16_t r, uint8_t color)
{
	updateBoundingBox(x0-r, y0-r, x0+r, y0+r);
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	uint16_t i;

	for (i=y0-r; i<=y0+r; i++)
	{
		EpdSetPixel(x0, i, color);
	}

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		for ( i=y0-y; i<=y0+y; i++)
		{
			EpdSetPixel(x0+x, i, color);
			EpdSetPixel(x0-x, i, color);
		}
		for ( i=y0-x; i<=y0+x; i++)
		{
			EpdSetPixel(x0+y, i, color);
			EpdSetPixel(x0-y, i, color);
		}
	}
}


NAN_METHOD(EpdLcd::Circle)
{
    Nan::HandleScope scope;

    unsigned int x = (info[0]->NumberValue());
    unsigned int y = (info[1]->NumberValue());
    unsigned int radius = (info[2]->NumberValue());

    EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());
    if (!info[3]->IsUndefined() && info[3]->BooleanValue() == true)
        LCDfillcircle(x, y, radius, obj->color);
    else
        LCDdrawcircle(x, y, radius, obj->color);
    return;
}

NAN_METHOD(EpdLcd::Font)
{
    Nan::HandleScope scope;

    EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());
    int err = 0;

    v8::String::Utf8Value fontName(info[0]->ToString());
    std::string _fontName = std::string(*fontName);
    uint8_t size = info[1]->IsUndefined() ? 8 : info[1]->NumberValue();

    //obj->fontSize = info[1]->IsUndefined() ? 12 : info[1]->NumberValue();
    //obj->fontBold = info[2]->IsUndefined() ? false : info[2]->BooleanValue();
    v8::Local<v8::Value> argv[1] = {Nan::New(DPI)};
    v8::Local<v8::Object> fontFaceWrapper = Nan::NewInstance(FontFace::GetConstructor(), 1, argv).ToLocalChecked();
    FontFace *fontFace = Nan::ObjectWrap::Unwrap<FontFace>(fontFaceWrapper);
    if ((err = FT_New_Face(obj->library, _fontName.c_str(), 0, &fontFace->face)))
    {
        fprintf(stderr, "Font load error: %d", err);
        Nan::ThrowError("Font load error");
    }
    info.GetReturnValue().Set(fontFaceWrapper);

    // << 6 because '26dot6' fixed-point format
    FT_Set_Char_Size(fontFace->face, size << 6, 0, DPI, 0);
    //FT_Set_Transform(obj->face, &obj->matrix, 0);
    return;
}

const FT_Matrix draw_bitmap(FT_Bitmap *bitmap,
                            FT_Int px,
                            FT_Int py, bool clear, bool invert)
{
    int x, y; 
    uint16_t bit = 0, byte = 0;
    FT_Matrix r;
    r.xx = px;
    r.yx = max(0, py);
    r.xy = px + bitmap->width;
    r.yy = py + bitmap->rows;
    updateBoundingBox(r.xx, r.yx, r.xy, r.yy);
    for (y = r.yx; y < r.yy; y++)
    {
        for (x = r.xx; x < r.xy; x++)
        {
            byte = (x - r.xx) / 8;
            bit = 0x80 >> ((x - r.xx) & 7);
            if (bitmap->buffer[(y - r.yx) * bitmap->pitch + byte] & bit)
            {
                EpdSetPixel(x, y, invert ? -1 : 1);
            }
            else if (clear)
            {
                EpdSetPixel(x, y, 0);
            }
        }
    }
    return r;
}

NAN_METHOD(EpdLcd::DrawChar)
{
    Nan::HandleScope scope;

    //EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());

    uint8_t err = 0;
    FontFace *fontFace = Nan::ObjectWrap::Unwrap<FontFace>(info[0]->ToObject());
    unsigned int x = (info[1]->NumberValue());
    unsigned int y = (info[2]->NumberValue());
    FT_Long c = (info[3]->NumberValue());
    bool clearChar = info[4]->IsUndefined() ? false : info[4]->BooleanValue();
    bool invert = info[5]->IsUndefined() ? false : (info[5]->BooleanValue());

    if (!c)
        return;
    FT_GlyphSlot slot = fontFace->face->glyph;
    if ((err = FT_Load_Char(fontFace->face, c, FT_LOAD_RENDER | FT_LOAD_MONOCHROME)))
    {
        return;
    }
    FT_Matrix rect = draw_bitmap(&slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top, clearChar, invert);
    v8::Local<v8::Object> bbox = Nan::New<v8::Object>();
    bbox->Set(Nan::New("xMin").ToLocalChecked(), Nan::New((int32_t)rect.xx));
    bbox->Set(Nan::New("xMax").ToLocalChecked(), Nan::New((int32_t)rect.xy));
    bbox->Set(Nan::New("yMin").ToLocalChecked(), Nan::New((int32_t)rect.yx));
    bbox->Set(Nan::New("yMax").ToLocalChecked(), Nan::New((int32_t)rect.yy));
    info.GetReturnValue().Set(bbox);
}

NAN_METHOD(EpdLcd::DrawString)
{
    Nan::HandleScope scope;

    //PcdLcd *obj = Nan::ObjectWrap::Unwrap<PcdLcd>(info.Holder());
    FontFace *fontFace = Nan::ObjectWrap::Unwrap<FontFace>(info[0]->ToObject());
    unsigned int x = (info[1]->NumberValue());
    unsigned int y = (info[2]->NumberValue());
    bool clearChar = info[4]->IsUndefined() ? false : info[4]->BooleanValue();
    bool invert = info[5]->IsUndefined() ? false : (info[5]->BooleanValue());

    v8::String::Utf8Value text(info[3]->ToString());
    std::string _text = std::string(*text);
    const char *str = _text.c_str();
    int err = 0;
    FT_Matrix rect;
    rect.xx = LCDWIDTH;
    rect.xy = 0;
    rect.yx = LCDHEIGHT;
    rect.yy = 0;
    unsigned int px = x, py = y;
    FT_GlyphSlot slot = fontFace->face->glyph;
    while (1)
    {
        char c = (*str++);
        if (!c)
            break;
        if ((err = FT_Load_Char(fontFace->face, c, FT_LOAD_RENDER | FT_LOAD_MONOCHROME)))
        {
            Nan::ThrowError("Loading character error");
        }
        FT_Matrix r = draw_bitmap(&slot->bitmap, px + slot->bitmap_left, py - slot->bitmap_top, clearChar, invert);
        if (rect.xx > r.xx)
            rect.xx = r.xx;
        if (rect.xy < r.xy)
            rect.xy = r.xy;
        if (rect.yx > r.yx)
            rect.yx = r.yx;
        if (rect.yy < r.yy)
            rect.yy = r.yy;
        px += slot->advance.x >> 6;
        py += slot->advance.y >> 6;

        //FT_Done_Glyph(slot);
    }
    v8::Local<v8::Object> bbox = Nan::New<v8::Object>();
    bbox->Set(Nan::New("xMin").ToLocalChecked(), Nan::New((int32_t)rect.xx));
    bbox->Set(Nan::New("xMax").ToLocalChecked(), Nan::New((int32_t)rect.xy));
    bbox->Set(Nan::New("yMin").ToLocalChecked(), Nan::New((int32_t)rect.yx));
    bbox->Set(Nan::New("yMax").ToLocalChecked(), Nan::New((int32_t)rect.yy));
    bbox->Set(Nan::New("px").ToLocalChecked(), Nan::New((int32_t)px));
    info.GetReturnValue().Set(bbox);
}

NAN_METHOD(EpdLcd::Image)
{
    Nan::HandleScope scope;

    /*unsigned char *buff = (unsigned char *)node::Buffer::Data(info[0]->ToObject());
    unsigned int x = info[1]->NumberValue();
    unsigned int y = info[2]->NumberValue();
    unsigned int w = info[3]->NumberValue();
    unsigned int h = info[4]->NumberValue();*/
    //bool invert = false;
    //updateBoundingBox(x, y, x+w, y+h);
    /*for (iy = y; iy < y+h; iy++)
    {
        for (ix = x; ix < x+w; ix++)
        {
            byte = (ix - x) / 8;
            bit = 0x80 >> ((ix - x) & 7);
            if (buff[(iy - y) + byte] & bit)
            {
                EpdSetPixel(ix, iy, invert ? -1 : 1);
            }
            else if (clear)
            {
                EpdSetPixel(ix, iy, 0);
            }
        }
    }*/
    //Dis_Drawing(x, y, (unsigned char *)buff, h, w);
    //v8::String::Utf8Value path(info[2]->ToString());
    //std::string _path = std::string(*path);

    //EpdLcd *obj = Nan::ObjectWrap::Unwrap<EpdLcd>(info.Holder());
    //LCDdrawImage(buffer);

    /*if (status != 0) {
        Nan::ThrowError("Error reading image");
    }*/

    return;
}

EpdLcd::EpdLcd(uint8_t channel, uint8_t dc, uint8_t cs, uint8_t rst, uint8_t led)
{
    this->channel = channel;
    for (long i = 0; i < LCDHEIGHT * LCDWIDTH * 8; i++)
    {
        lcd_buff[i] = 0xff;
    }
    if (wiringPiSetupGpio() == -1)
    {
        Nan::ThrowError("Error opening gpio");
    }
    //2.GPIO config
    pinMode(RST, OUTPUT);
    pinMode(DC, OUTPUT);
    pinMode(BUSY, INPUT);
    pinMode(CS, OUTPUT); //must set cs output,Otherwise it does not work
    if (led < 127) {
        pinMode(led, OUTPUT);
    }

    //3.spi init
    if (wiringPiSPISetup(channel, 2000000) < 1)
    {
        Nan::ThrowError("Error opening spi lcd");
    }

    EPD_RST_1;
    driver_delay_xms(10);
    EPD_RST_0;
    driver_delay_xms(10);
    EPD_RST_1;

    this->matrix.xx = 0;
    this->matrix.xy = 1 << 16;
    this->matrix.yx = -1;
    this->matrix.yy = 0;
    //LCDclear();
    int err = 0;
    if ((err = FT_Init_FreeType(&library)))
    {
        FT_Done_FreeType(library);
        Nan::ThrowError("FreeType init error");
    }
}

EpdLcd::~EpdLcd()
{
    FT_Done_FreeType(library);
}
