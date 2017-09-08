
#include <ft2build.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */
//#include <math.h>

#include FT_GLYPH_H
#define DPI 100 // Approximate res. of Adafruit 2.8" TFT

#define WIDTH   84
#define HEIGHT  48


/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

const void draw_bitmap( FT_Bitmap*  bitmap,
             FT_Int      px,
             FT_Int      py)
{
  FT_Int  i, j, p, q;
  FT_Int  x_max = px + bitmap->width;
  FT_Int  y_max = py + bitmap->rows;
  printf("draw (%d,%d)->(%d,%d)\n", px, py, x_max, y_max);
  int bit = 0;
  int byte = 0;
  uint8_t x,y;
  for(y=py; y < y_max; y++) {
			for(x=px;x < x_max; x++) {
				byte = (x-px) / 8;
				bit  = 0x80 >> ((x-px) & 7);
				image[y][x] = (bitmap->buffer[(y-py) * bitmap->pitch + byte] & bit);
			}
		}
}
const void show_image( void )
{
  int  i, j;


  for ( i = 0; i < HEIGHT; i++ )
  {
    for ( j = 0; j < WIDTH; j++ )
      putchar( image[i][j] == 0 ? ' ' : '*' );
      putchar( '\n' );
  }
}

int main(int argc, char *argv[]) {
	int                i, j, err, size, first=' ', last='~',
	                   bitmapOffset = 0, x, y, byte;
	char              *fontName, c, *ptr;
	FT_Library         library;
	FT_Face            face;
	FT_Glyph           glyph;
	FT_Bitmap         *bitmap;
	FT_BitmapGlyphRec *g;
     FT_Matrix     matrix;  
	uint8_t            bit;
	if((err = FT_Init_FreeType(&library))) {
		fprintf(stderr, "FreeType init error: %d", err);
		return -1;
	}
	if((err = FT_New_Face(library, "../test/fontawesome-webfont.ttf", 0, &face))) {
		fprintf(stderr, "Font load error: %d", err);
		FT_Done_FreeType(library);
	}	
	// << 6 because '26dot6' fixed-point format
	FT_Set_Char_Size(face, 16 << 6, 0, DPI, 0);
    //FT_Set_Transform(face, &matrix, 0);
    const char *str = "1";
    fprintf(stdout, "Begin\n");
    //show_image();
    
    uint8_t px = 0;
    while (1)
	{
		FT_ULong c = 0xf188;//(*str++);
    if ((c & 0x80) == 0x00) {
      printf("utf8\n");
    }
    else if ((c & 0xE0) == 0xC0) {
      printf("utf16\n");
    } else {
      printf("utf32\n");
    }
        fprintf(stdout, "Char: %d\n", c);
		if (!c)
			break;
        FT_GlyphSlot slot = face->glyph;
        if((err = FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_MONO ))) {
    	    fprintf(stderr, "Error %d loading char '%c'\n",
    	      err, c);
    	    continue;
        }
    
       /* if((err = FT_Render_Glyph(face->glyph,
          FT_RENDER_MODE_MONO))) {
    	    fprintf(stderr, "Error %d rendering char '%c'\n",
    	      err, c);
    	    continue;
        }
    
        if((err = FT_Get_Glyph(face->glyph, &glyph))) {
    	    fprintf(stderr, "Error %d getting glyph '%c'\n",
    	      err, c);
    	    continue;
        }
        if ((err = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, 0, 0))) {
    	    fprintf(stderr, "Error %d Rendering '%c'\n",
    	      err, c);
            continue;
        }*/
        uint8_t bit;
        
        fprintf(stdout, "Char loaded: %c, %d\n", c, px);
        draw_bitmap( &slot->bitmap,
                 px + slot-> bitmap_left,
                 0 );
        px += slot->advance.x >> 6;
        break;
       /* FT_BitmapGlyph slot = (FT_BitmapGlyph)glyph;
        bitmap = slot->bitmap;
        int pitch = abs(bitmap.pitch);
        uint16_t i = 0;
        uint8_t x;
        uint8_t y;*/
        /*for (i = 0; i <  bitmap.width*bitmap.rows/8; i++) {
           // printf("0x%02X\n",bitmap.buffer[i]);
           printf("%s\n", byte_to_binary(bitmap.buffer[i]));
        }*/
        /*for(y=0; y < bitmap.rows; y++) {
    	    for(x=0;x < bitmap.width; x++) {
                unsigned char *row = &bitmap.buffer[pitch * y];
                char cValue = row[x >> 3];
                printf("0x%02X", cValue);
               //fprintf(stdout, "%d -> %d,%d\n", cValue, x, y);
            }
        }*/
    }
  FT_Done_Face    ( face );
	FT_Done_FreeType(library);
  show_image();
}