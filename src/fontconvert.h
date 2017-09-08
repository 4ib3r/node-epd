#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <ft2build.h>
#include FT_GLYPH_H
#define DPI 141 // Approximate res. of Adafruit 2.8" TFT

public class FontConverter {
    private:
    int                i, j, err, size, first=' ', last='~',
	                   bitmapOffset = 0, x, y, byte;
	char              *fontName, c, *ptr;
	FT_Library         library;
	FT_Face            face;
	FT_Glyph           glyph;
	FT_Bitmap         *bitmap;
	FT_BitmapGlyphRec *g;
	GFXglyph          *table;
	uint8_t            bit;

    public:
        void loadFont();

}
