Esp32cam security camera

This repo is an implementation of an small security camera that takes snapshots every 1 secs and then
save the image to the sd card, the device wont got into sleep mode so use a 5000 mAh battery pack as a minimum power source, for more info about the esp32cam check the oficial repo https://github.com/espressif/esp32-camera 

v0.2 
--added wifi conection in station mode, add your own ssid and password on start_server.h
--fixed an issue when writing to log file
--moved several extra functions to otros.c

TODO
--send the image data to rpi3 via http get/post
--remove image saving to file, deprecated
--increase frame rate to get best image quality

before you take any photo:
    add esp32cam-camera repo into components folders if you dont have it, if not you would have lots of headaches
    enable psram in the menu config for the camera to work
    set the long filename buffer in heap to yes, so you can use long filenames,  FATFS_LFN_STACK
    pixel_format= set the compresion type of the jpg file, PIXFORMAT_JPEG is the most ussual
    frame_size = set the resolution of the image taken, check sensor.h for more info about image sizes

board = Esp32cam Ai thinker
framework used = espressif
