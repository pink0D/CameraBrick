# CameraBrick
ESP32-CAM adaptation for use as an FPV camera with LEGO Technic / Power Functions.
This repo includes PCB instructions and modified code from CameraWebServer. 
Main differences include:
1. Lowest possible camera resoulution for highest possible framerate. Maximum FPS is dependend mainly on WiFi signal level and the hub's ability to provide enough electrical power to the camera module.
2. mDNS so you can reach the video server by hostname

[YouTube demo video](https://www.youtube.com/watch?v=FWp9zUCGctc)

# Important notice

1. ESP32-CAM consumes quite a lot of electrical power and is very sensitive to undervoltage. Most LEGO compatible Power Functions remote hubs are able to provide required power, however if your vehicle also contains a lot of motors, you can drain the hub's battery very fast or even burn it. Think of a connected ESP32-Cam as if it is a constantly rotating L-motor at full speed.
2. ESP32-CAM module becomes very hot very quickly. Do not touch it while it is working and immediatly after powering off.

# Materials needed

1. ESP32-CAM board and camera
2. ESP32-CAM-MB or other USB-Serial adapter for uploading the sketch
3. 9V to 5V buck converter (N7805 or similar, capable to deliver 5V/1A). **Do not use older LM7805 since it is very inefficient - the power from the hub will be wasted, resulting in 1) even more extreme heating 2) quick battery drain 3) lower FPS due to undervoltage 4) possible damaging of the hub/battery**
4. 30x70mm PCB
5. 2x female 8-pin headers
6. Power Functions extension cable
7. (optional) Any kind of 2.54 4-wire connectors to solder on PCB and extension cable to make it detachable.
8. Some wires and other soldering accessories

## PCB schematic
![wiring!](https://github.com/pink0D/CameraBrick/blob/main/fpv_pf_bb.png?raw=true "Wiring")

## Power Functions connector pinout
Before soldering, please review the pinout of the Power Functions Connector [Power Functions](https://www.philohome.com/pf/pfcon.jpg)
![PF Connector!](https://www.philohome.com/pf/pfcon.jpg "PF Connector")

# Uploading the sketch
1. Install ESP32 Boards in Arudino IDE [https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
2. Download sketch code from this repo
3. Update **secrets.h** with your WiFi credentials
4. *(optional)* Update **esp_camera_server.h** if you are usign a different camera. AI Thinker is the default option
5. *(optional)* Update hostname in main sketch file. **fpvbrick** is the default hostname
6. Set board to ESP32 Dev Module and enable PSRAM in Tools menu
7. Upload the sketch to ESP32CAM either using [ESP32-CAM-MB](https://randomnerdtutorials.com/upload-code-esp32-cam-mb-usb/) or [USB-Serial adapter](https://randomnerdtutorials.com/program-upload-code-esp32-cam/)
   
# Viewing the video
There are two main options
1. Open [http://fpvbrick.local](http://fpvbrick.local) in desktop or mobile browser
2. Open [http://fpvbrick.local:81/stream](http://fpvbrick.local:81/stream) in video player that supports streaming ([VLC media player](https://www.videolan.org/vlc/) for example)
