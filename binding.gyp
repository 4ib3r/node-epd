{
  "targets": [
    {
      "target_name": "epd",
      "sources": [
        "src/lcd.cc", "src/lcd_tft.cc", "src/epd_driver.cc", "src/FontFace.cc" ],
      "cflags": ["-Wfatal-errors", "-Wdeprecated-declarations", "-Wunused-variable"],
      "include_dirs": [ "<!(node -e \"require('nan')\")", '/usr/include/freetype2'],
      'libraries': [
        '-L/usr/local/lib', '-lwiringPi', '-lfreetype'
      ]
    }
  ]
}