# PlatformIO
This is made for use with PlatformIO. PIO can be easily installed as an extension to VSCode.
https://platformio.org/install/ide?install=vscode

# Instructions
1. Specify the required build flags in platformio.ini. These build flags are listed later in this document.
2. 

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
| - | `N_LEDS` | `DNLEDS=16` | How many status light LEDS the device has. Usually 1 for interlocks and 16 for doors.
| - | `DEVICE_NAME` | `-DDEVICE_NAME="\"FrontDoor\""` | The name of the device, e.g. "FrontDoor". This can be anything (don't use spaces) but make it descriptive. |
| - | `WIFI_SSID` | `-DWIFI_SSID="\"HSBNEInfra\""` | The WiFi SSID. Please use HSBNEInfra. |
| - | `WIFI_PASSWORD` | -DWIFI_PASSWORD="\"MyWiFiPassword\"" | The password for the WiFi. |
| - | `HOST_ADDRESS` | `-DHOST_ADDRESS="\"https://portal.hsbne.org\""` | The host address for the interlock ot talk to. Probably `https://portal.hsbne.org`. Do not include a trailing `/`.
| - | `HOST_SECRET` | `-DHOST_SECRET="\"MyHostSecret\""` | ~ |
| - | `OTA_PASSWORD` | `-DOTA_PASSWORD="\"MyOTAPasswrod\""` | The password to be used for Over The Air (OTA) firmware updates. __Please do not make one up, use the established password system.__ |
| - | `SKELETON_CARD` | `-DSKELETON_CARD=1234` | An RFID number that will always unlock this door/interlock.

There is some additional configuration that can be done by making edits to `include/Core.h`

# Thanks to
Thanks to nog3 for the development of the original firmware.
https://github.com/HSBNE/AccessControl