# node-epd
Nodejs library  for Waveshare e-Paper modules

# Information
Library is writed for 2.9" waveshare e-paper display, but it should work for 1.54", 2.13".

# Requirements
* wiringPi
* libfreetype6-dev

# Example
```javascript var Lcd = require('node-epd');
var lcd = new Lcd(0, 4, 10, 5, 1, 55); //initialize display
lcd.clear(); //clear all lcd
var fontAwesome = lcd.font('./test/fontawesome-webfont.ttf', 10); //load ttf font
var robotoFont = lcd.font('./test/Roboto-Regular.ttf', 14); //load ttf font
lcd.char(fontAwesome, 50, 110, 0xf206); //draw char (utf16)
lcd.drawString(robotoFont, 10, 50, "Hello"); //draw string (utf8)
lcd.circle(10,20,25, true); //draw filled circle
lcd.rect(50, 8, 50, 50); //draw rect
lcd.update(function() { //update all display
  console.log("LCD updated");
  lcd.clearPart(); //switch to partitial update mode
  lcd.rect(100, 100, 20, 20); //draw rect
  lcd.line(100, 100, 120, 120); //draw line in rect
  lcd.partUpdate(); //update only changed part of display
});
```

# Documentation

## Classes

<dl>
<dt><a href="#Lcd">Lcd</a></dt>
<dd><p>Main Lcd driver class</p>
</dd>
</dl>

## Typedefs

<dl>
<dt><a href="#Rect">Rect</a> : <code>Object</code></dt>
<dd><p>Rect object</p>
</dd>
<dt><a href="#FontFace">FontFace</a> : <code>Object</code></dt>
<dd><p>FT_Face object wrapper</p>
</dd>
<dt><a href="#EpdSize">EpdSize</a> : <code>Object</code></dt>
<dd><p>EpdSize object</p>
</dd>
<dt><a href="#Color">Color</a> : <code>number</code></dt>
<dd><p>Drawing color -1 inverse, 1 black (default), 0 white</p>
</dd>
</dl>

<a name="Lcd"></a>

## Lcd
Main Lcd driver class

**Kind**: global class  

* [Lcd](#Lcd)
    * [new Lcd(channel, dc, cs, rst, led)](#new_Lcd_new)
    * [.font(ttfPath, font)](#Lcd+font) ⇒ [<code>FontFace</code>](#FontFace)
    * [.size()](#Lcd+size) ⇒ [<code>EpdSize</code>](#EpdSize)
    * [.clear()](#Lcd+clear)
    * [.clearPart()](#Lcd+clearPart)
    * [.update()](#Lcd+update)
    * [.partUpdate()](#Lcd+partUpdate)
    * [.line(x, y, x2, y2, col)](#Lcd+line)
    * [.rect(x, y, width, height, fill, col)](#Lcd+rect)
    * [.circle(x, y, r, fill, col)](#Lcd+circle)
    * [.pixel(x, y, col)](#Lcd+pixel)
    * [.drawChar(x, y, char)](#Lcd+drawChar) ⇒ [<code>Rect</code>](#Rect)
    * [.drawString(font, x, y)](#Lcd+drawString) ⇒ [<code>Rect</code>](#Rect)
    * [.color(color)](#Lcd+color)

<a name="new_Lcd_new"></a>

### new Lcd(channel, dc, cs, rst, led)

| Param | Type | Description |
| --- | --- | --- |
| channel | <code>number</code> | spi channel |
| dc | <code>number</code> | dc pin |
| cs | <code>number</code> | cs pin |
| rst | <code>number</code> | rst pin |
| led | <code>number</code> | additional led pin |

<a name="Lcd+font"></a>

### lcd.font(ttfPath, font) ⇒ [<code>FontFace</code>](#FontFace)
Load ttf font

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  
**Returns**: [<code>FontFace</code>](#FontFace) - Description of return value.  

| Param | Type | Description |
| --- | --- | --- |
| ttfPath | <code>string</code> | font path |
| font | <code>number</code> | size |

<a name="Lcd+size"></a>

### lcd.size() ⇒ [<code>EpdSize</code>](#EpdSize)
Return lcd resolution

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  
**Returns**: [<code>EpdSize</code>](#EpdSize) - display width and height  
<a name="Lcd+clear"></a>

### lcd.clear()
Clear whole screen and set all refresh mode

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  
<a name="Lcd+clearPart"></a>

### lcd.clearPart()
Change display mode to partial drawing

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  
<a name="Lcd+update"></a>

### lcd.update()
Update whole screen

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  
<a name="Lcd+partUpdate"></a>

### lcd.partUpdate()
Update changed part of screen

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  
<a name="Lcd+line"></a>

### lcd.line(x, y, x2, y2, col)
Draw line

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  

| Param | Type | Description |
| --- | --- | --- |
| x | <code>number</code> | start point on x axis |
| y | <code>number</code> | start point on y axis |
| x2 | <code>number</code> | end point on x axis |
| y2 | <code>number</code> | end point on y axis |
| col | [<code>Color</code>](#Color) | line color |

<a name="Lcd+rect"></a>

### lcd.rect(x, y, width, height, fill, col)
Draw rect

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  

| Param | Type | Description |
| --- | --- | --- |
| x | <code>number</code> | start point on x axis |
| y | <code>number</code> | start point on y axis |
| width | <code>number</code> | rect width |
| height | <code>number</code> | rect height |
| fill | <code>bool</code> | If true rect is filled |
| col | [<code>Color</code>](#Color) | rect color |

<a name="Lcd+circle"></a>

### lcd.circle(x, y, r, fill, col)
Draw circle

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  

| Param | Type | Description |
| --- | --- | --- |
| x | <code>number</code> | mid point on x axis |
| y | <code>number</code> | mid point on y axis |
| r | <code>number</code> | circle size |
| fill | <code>bool</code> | If true circle is filled |
| col | [<code>Color</code>](#Color) | circle color |

<a name="Lcd+pixel"></a>

### lcd.pixel(x, y, col)
Draw pixel

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  

| Param | Type | Description |
| --- | --- | --- |
| x | <code>number</code> | point on x axis |
| y | <code>number</code> | point on y axis |
| col | [<code>Color</code>](#Color) | pixel color |

<a name="Lcd+drawChar"></a>

### lcd.drawChar(x, y, char) ⇒ [<code>Rect</code>](#Rect)
Draw character from 16 bit representation

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  

| Param | Type | Description |
| --- | --- | --- |
| x | <code>number</code> | point on x axis |
| y | <code>number</code> | point on y axis |
| char | <code>number</code> | char code |

<a name="Lcd+drawString"></a>

### lcd.drawString(font, x, y) ⇒ [<code>Rect</code>](#Rect)
Draw string from 8bit representation

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  
**Returns**: [<code>Rect</code>](#Rect) - drawed coordinates  

| Param | Type | Description |
| --- | --- | --- |
| font | [<code>FontFace</code>](#FontFace) | Font to draw string |
| x | <code>number</code> | x position |
| y | <code>number</code> | y position |

<a name="Lcd+color"></a>

### lcd.color(color)
Set default color to use

**Kind**: instance method of [<code>Lcd</code>](#Lcd)  

| Param | Type | Description |
| --- | --- | --- |
| color | <code>number</code> | 0 is white, 1 is black, -1 is inversion of current state |

<a name="Rect"></a>

## Rect : <code>Object</code>
Rect object

**Kind**: global typedef  
**Properties**

| Name | Type |
| --- | --- |
| xMin | <code>number</code> | 
| yMin | <code>number</code> | 
| xMax | <code>number</code> | 
| yMax | <code>number</code> | 

<a name="FontFace"></a>

## FontFace : <code>Object</code>
FT_Face object wrapper

**Kind**: global typedef  
**Properties**

| Name | Type | Description |
| --- | --- | --- |
| size | <code>number</code> | font size |
| height | <code>number</code> | font size in px |

<a name="EpdSize"></a>

## EpdSize : <code>Object</code>
EpdSize object

**Kind**: global typedef  
**Properties**

| Name | Type | Description |
| --- | --- | --- |
| width | <code>number</code> | Display width in px |
| height | <code>number</code> | Display height in px |

<a name="Color"></a>

## Color : <code>number</code>
Drawing color -1 inverse, 1 black (default), 0 white

**Kind**: global typedef  
