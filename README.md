# PlatformIO
This is made for use with PlatformIO. PIO can be easily installed as an extension to VS Code.
https://platformio.org/install/ide?install=vscode

# Instructions for uploading over Serial/FTDI
1. Clone this repo and open it with VS Code with the PlatformIO extension installed `git clone https://github.com/Ryan-Kirkpatrick/Interlock2.git`
2. Edit platformio.ini and specify the required build flags under `env:sonoff_th`. The build flags are described later in this document. As an alternative you can directly edit `include/Core.h` and `include/Secrets.h`.
3. Using the PlatformIO VS Code extension ,select `env:sonoff_th` as the environment. This can be done by pressing the folder icon on the blue bar at the bottom of the window.
4. __DISCONNECT THE INTERLOCK FROM MAINS POWER.__
5. If your FTDI is already plugged into your computer disconnect it.
6. Connect the GND, RX, TX, and VCC pins of the Sonoff to the FTDI. __Ensure that the voltage on any pin does not exceed 3.3v and that the GND pin is connected correctly. The Sonoff will be damaged by exceeding 3.3v or by improper grounding.__
7. While holding down the flash button on the Sonoff (the big button that sticks up) plug the FTDI into your computer. After the FTDI is plugged in, release the flash button. There should be no LEDs lit up or blinking on the Sonoff.
8. Upload the firmware using PlatformIO. Using the PlatformIO VS Code extension this can be done by pressing the upload button on the blue bar at the bottom of the window. PlatformioIO should automatically select the correct upload port.
9. Done. The interlock must now be assigned a static IP address and setup by a portal admin.

# Build flags
Use exactly one build flag from each category. Use all build flags with an unspecified category.

|Category | Flag | platformio.ini | Description |
|-|-|-|-|
| RFID | `RF125PS_READER` | `-DRF125PS_READER` | The interlock uses the newer RF125PS RFID reader hardware. Use this flag for new interlocks.
| RFID | `OLD_READER` | `-DOLD_READER` | The interlock uses the old RFID reader. There are almost none of these readers left in use. |
| LED | `GRBW` | `-DGRBW` | The staaus LED used a Green Red Blue White color scheme. Used on newer interlocks |
| LED | `RGBW` | `-DRGBW` | The status LED uses a Red Green Blue White color scheme. Usually used on older interlocks. |
| Type | `DOOR` | `-DDOOR` | This device is a door. |
| Type | `INTERLOCK` | `-DINTERLOCK` | This device is an interlock |
| - | `N_LEDS` | `-DNLEDS=16` | How many status light LEDS the device has. Usually 1 for interlocks and 16 for doors.
| - | `DEVICE_NAME` | `-DDEVICE_NAME="\"FrontDoor\""` | The name of the device, e.g. "FrontDoor". This can be anything (don't use spaces) but make it descriptive. |
| - | `WIFI_SSID` | `-DWIFI_SSID="\"HSBNEInfra\""` | The WiFi SSID. Please use `HSBNEInfra`. |
| - | `WIFI_PASSWORD` | `-DWIFI_PASSWORD="\"MyWiFiPassword\""` | The password for the WiFi. |
| - | `HOST_ADDRESS` | `-DHOST_ADDRESS="\"https://portal.hsbne.org\""` | The host address for the interlock ot talk to. Probably `https://portal.hsbne.org`. Do not include a trailing `/`.
| - | `HOST_SECRET` | `-DHOST_SECRET="\"MyHostSecret\""` | ~ |
| - | `OTA_PASSWORD` | `-DOTA_PASSWORD="\"MyOTAPassword\""` | The password to be used for Over The Air (OTA) firmware updates. __Please do not make one up, use the established password system.__ |
| - | `SKELETON_CARD` | `-DSKELETON_CARD=1234` | An RFID number that will always unlock this door/interlock.

There is some additional configuration that can be done by making edits to `include/Core.h`

# Thanks to
Thanks to nog3 for the development of the original interlock firmware.
https://github.com/HSBNE/AccessControl