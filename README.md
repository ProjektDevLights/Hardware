<p align="center"><img alt="Logo" src="https://i.postimg.cc/vHgyC8nG/logo.png" height="250" /> </p>

### Description
DevLights are the smart home LED Stripes for Developers.
This is the code for the DevLights Hardware intended to run on an [ESP8266](https://www.espressif.com/en/products/socs/esp8266) or preferably the custom designed boards.

### Prerequisites
Make sure that you have installed the Arduino IDE and [added](https://arduino-esp8266.readthedocs.io/en/latest/installing.html) the [ESP8266 platform](https://arduino.esp8266.com/stable/package_esp8266com_index.json) to it. 

### Getting started
```shell
git clone https://github.com/ProjektDevLights/Hardware.git
cd Hardware
arduino . #or open the folder from the ide (on windows)
```

Install the following libraries
- [ArduinoJson](https://arduinojson.org/)
- [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient)
- [StringSplitter](https://github.com/aharshac/StringSplitter)
- [AdafruitNeopixel](https://github.com/adafruit/Adafruit_NeoPixel)

To compile and upload the code to the ESP connect it to your PC and hit the upload button in the IDE.

### Contributing
Feel free to open an issue on GitHub or send pull requests, when encountering issues or want to add some features. If you have any questions email [peters@teckdigital.de](mailto:peters@teckdigital.de) or [slaar@teckdigital.de](mailto:slaar@teckdigital.de).
