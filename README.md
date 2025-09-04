# Dot Matrix Clock

In this project, we are making a clock that displays the time and date on a 64x32 dot matrix panel. The clock is powered by an ESP32 microcontroller. The time is calibrated by NTP servers over Internet. Optionally, we use an RTC module to keep the time when we're offline or during power outages.

## Hardware

### Dot Matrix Panel

Most of these dot matrix panels are simply powered by [shift registers](https://learn.sparkfun.com/tutorials/shift-registers/all) and are chainable to make bigger panels. I bought [this one](https://www.waveshare.com/wiki/RGB-Matrix-P5-64x32) from AliExpress. I'm not sure if it was a knock-off but it works and it's cheap.

### Microcontroller

Since we want to sync time with NTP servers, we need a microcontroller with WiFi capabilities. ESP32 is a natrual choice. We also need enough pins to drive the dot matrix panel. I have a few ESP32-WROOM dev boards and decided to use one of them.

### RTC module

DS3231 RTC

## Software

### Driving the dot matrix display

I found two libraries, both Adafruit GFX compatible.

* [Protomatter](https://docs.arduino.cc/libraries/adafruit-protomatter/)
* [ESP32 HUB75 LED MATRIX PANEL DMA Display](https://docs.arduino.cc/libraries/esp32-hub75-led-matrix-panel-dma-display/)

### WiFi

### NTP

### RTC
https://randomnerdtutorials.com/esp32-ds3231-real-time-clock-arduino/#intro-rtc

## Assembly