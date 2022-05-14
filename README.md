# WIFIDuino-Sensor-MQTT
This project is from my Graduation Thesis "Single-chip Soil Temperature and Humidity Detection System Design Based on Wi-Fi Technology".

This repo contains source code and PCB Design of the Soil temperature and humidity detector prototype based on Arduino and MQTT Protocol, companion Android App developed via MIT App Inventor 2.

<br>

## What did my thesis do?

- A Wi-Fi based microcontroller soil temperature and humidity detection system design
- A MQTT proxy server solution using Raspberry Pi 3B
- Drew the corresponding PCB board using Altium Designer 16
- A companion application for Android platform was written to interact with the detection system

<br>

## Selection of the prototype parts

- Arduino UNO (actually, the WIFIduino board is used as the microcontroller)
- ESP8266-12F Wi-Fi module, which is embedded into WIFIduino
- DS18B20 single-wire waterproof temperature sensor, requires a special adapter module.
- Capacitive soil moisture sensor
- DHT-11 ambient temperature and humidity sensor
- Raspberry Pi 3B, used as MQTT proxy server
- Android system smartphone, used as a host computer for receiving data
