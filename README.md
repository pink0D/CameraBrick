# Overview
**CameraBrick** is an Arduino library for building WiFi FPV controllers for Mould King Power Modules. 

# The concept

picture
<img width="800" alt="overview" src="https://github.com/user-attachments/assets/687a52b4-1883-42d5-83dd-2bec049c402b" />


# Supported ESP32 boards

|  Board  | Status   |
| :---           |    :----        |   
| **Generic ESP32CAM**  (a.k.a. AI Thinker) | :white_check_mark: Supported, tested with OV2640 (OV3660 should work as well) |
| [M5Stack AtomS3R-M12](https://docs.m5stack.com/en/core/AtomS3R-M12)   | :white_check_mark: Supported |
| [M5StackAtomS3R-CAM](https://docs.m5stack.com/en/core/AtomS3R%20Cam)  | :white_check_mark: Supported (possible lower framerate)|
| [M5Stack TimerCamera-F](https://docs.m5stack.com/en/unit/timercam_f)  | :white_check_mark: Supported |
| [M5Stack TimerCamera-X](https://docs.m5stack.com/en/unit/timercam_x)   | :white_check_mark: Supported |

# Supported Mould King modules

CameraBricks supports MK4 (4-channel) and MK6 (6-channel) power modules controlled with simple Bluetooth LE advertising ([MouldKingino](https://github.com/pink0D/MouldKingino)).

<img width="50" alt="MK logo" src="https://github.com/user-attachments/assets/7d436cb4-5eb3-4f68-9fc1-404786991aca" />

Only MK modules are supported - other manufacturer use different protocols not yet implemented for Arduino.

|  MK module  | Status   |
| :---           |    :----        |   
| **Mould King 4.0 Power Module**  <br/> <img width="200" alt="mk40" src="https://github.com/user-attachments/assets/c3ec0708-9cdb-4308-a025-21dcd572bf07" /> | :white_check_mark: Supported |
| **Mould King 6.0 Power Module**  <br/> <img width="200" alt="mk60" src="https://github.com/user-attachments/assets/b88d586e-67f1-4b0c-a7f2-5ec4de91fed2" /> | :white_check_mark: Supported |


# License & credits
- CameraBrick is open source and licensed under the MIT License

# Contacts
Issues: [CameraBrick issues](https://github.com/pink0D/CameraBrick/issues)

Discussions: [CameraBrick discussions](https://github.com/pink0D/CameraBrick/discussions)

EMail: [pink0D.github@gmail.com](mailto:pink0d.github@gmail.com)
