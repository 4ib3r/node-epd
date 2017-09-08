# node-epd
Nodejs library  for Waveshare e-Paper modules

## Information
Library is writed for 2.9" waveshare e-paper display, but it should work for 1.54", 2.13".

## Requirements
* wiringPi
* libfreetype6-dev

## Example
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
