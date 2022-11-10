<p style="text-align:center;"><img src="/pictures/IMG_0196.jpeg" alt="picture" width="480"></p>
# IZ7BOJ-ESP32_RF_ATTENUATOR
RF attenuator based on Peregrine PE4302 and ESP32

Simple 0-31.5dB (step 0.5dB) attenuator with Wi-Fi remote control capability, based on ESP32 WROOM evaluation board and PE4302 attenuator. Maximum working frequency is 4GHz (see manufacturer datasheet for further details).

The attenuation can be set locally by Up/Down buttons or remotely via web page.

The attenuation is shown on a OLED 128x64 display, connected on the I2C bus.

I used a prototype board and wire-wrap wires (see picture), but a PCB is in the "to do list".

The OLED display is powered by the 3V3 provided by the ESP32, and its SCL and SDA lines are wired respectively to pins 39 and 42 of ESP32.

The Up and Down Buttons are connected respectively to pins 35,34 of ESP32, and pull-upped to 3V3 through 10K resistors. The buttons close the lines to ground when pressed.

The bit from LSB to MSB of the PE4302 are connected to pins 23,25,26,27,33,32 of ESP32, ant the 3V3 is taken from ESP32 too.

By the way, all the pin assignment can be changed in the code and on the PCB, if you want.

Note: The Wi-Fi SSID and password must be declared in the .ino file , "network credential" section, before uploading the code in the ESP32.

For further information, write at IZ7BOJ [at] gmail.com
