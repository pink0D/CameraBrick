# CameraBrick
ESP32-CAM adaptation for use as an FPV camera with LEGO Technic / Power Functions.
This repo includes PCB files and modified code from CameraWebServer. 
Main differences include:
1. Lowest possible camera resoulution for highest possible framerate. Maximum FPS is dependend mainly on WiFi signal level and the hub's ability to provide enough electrical power to the camera module.
2. mDNS so you can reach the video server by hostname

[YouTube demo video - FPV](https://www.youtube.com/watch?v=FWp9zUCGctc)

# GeekServo adapter
This PCB and sketch can be also used as an adapter to control a GeekServo with a **Mould King Power Module 4.0**. You can use just the servo functionality without connecting the OV2640 camera module (but ESP32-CAM board is still required).

[YouTube demo video - GeekServo](https://www.youtube.com/watch?v=FWp9zUCGctc)

# Important notice

1. The camera module consumes quite a lot of electrical power and is very sensitive to undervoltage. Most LEGO compatible Power Functions remote hubs are able to provide required power, however if your vehicle also contains a lot of motors, you can drain the hub's battery very fast or even burn it. Think of a connected cammera as if it is a constantly rotating L-motor at full speed.
2. The module becomes very hot very quickly. Do not touch it while it is working and immediatly after powering off.

# Materials needed

1. ESP32-CAM board
2. (optional - only needed for the FPV) OV2640 camera module
3. ESP32-CAM-MB or other USB-Serial adapter for uploading the sketch
4. 9V to 5V buck converter (N7805 or similar, capable to deliver 5V/1A for the camera module). **Do not use older LM7805 for powering the camera since it is very inefficient - the power from the hub will be wasted, resulting in 1) even more extreme heating 2) quick battery drain 3) lower FPS due to undervoltage 4) possible damaging of the hub/battery.** If the board is used just as a GeekServo adapter (without enabling the FPV functionality) you can use a simple LM7805, which is sufficient for a servo.
5. 30x70mm PCB breadboard or factory made PCB from files from this repo
6. 2x female 8-pin headers 
8. Power Functions extension cable
9. (optional) Any kind of 2.54 4-wire connectors to solder on PCB and extension cable to make it detachable.
10. (optional - only needed for GeekServo adapter) 2x 1 kOhm and 2x 2.2k Ohm resistors 
11. (optional - only needed for GeekServo adapter) 3-pin male pins for attaching the GeekServo 
12. Some wires and other soldering accessories

## PCB schematic for FPV
![wiring!](https://github.com/pink0D/CameraBrick/blob/main/Schematics/fpv_pf_bb.png?raw=true)

## PCB schematic for GeekServo
![wiring!](https://github.com/pink0D/CameraBrick/blob/main/Schematics/fpv_geek_bb.png?raw=true)
If you experience power related issues, try adding [capacitors](https://github.com/pink0D/CameraBrick/blob/main/Schematics/fpv_geek_caps.png?raw=true)

## Production PCB
You can order a factory made PCB to reduce soldering. Production files are located in PCB directory of this repo.

## Power Functions connector pinout
Before soldering, please review the pinout of the Power Functions Connector [Power Functions](https://www.philohome.com/pf/pfcon.jpg)
![PF Connector!](https://www.philohome.com/pf/pfcon.jpg "PF Connector")

# Uploading the sketch
1. Install ESP32 Boards in Arudino IDE [https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
2. Download sketch code from this repo
3. Install [ESP32Servo](https://docs.arduino.cc/libraries/esp32servo/) library dependency
4. Review the defines in the beginning of the main **CameraBrick.ino**. You can enable FPV and GeekServo functionality independently or use both at the same time.
5. Update **secrets.h** with your WiFi credentials
6. *(optional)* Update **esp_camera_server.h** if you are using a different board or camera model. AI Thinker & OV2640 are the default options
7. Set board to ESP32 Dev Module and enable PSRAM in Tools menu
8. Upload the sketch to ESP32CAM either using [ESP32-CAM-MB](https://randomnerdtutorials.com/upload-code-esp32-cam-mb-usb/) or [USB-Serial adapter](https://randomnerdtutorials.com/program-upload-code-esp32-cam/)
   
# Viewing the video
There are two main options
1. Open [http://fpvbrick.local](http://fpvbrick.local) in desktop or mobile browser
2. Open [http://fpvbrick.local:81/stream](http://fpvbrick.local:81/stream) in video player that supports streaming ([VLC media player](https://www.videolan.org/vlc/) for example)
