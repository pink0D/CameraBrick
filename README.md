# Overview
**CameraBrick** is an Arduino library for building WiFi FPV controllers for Mould King Power Modules. 

# Demo

[YouTube demo](https://www.youtube.com/watch?v=O1w-PtrD3Yg)

# The concept

<img width="800" alt="overview" src="https://github.com/user-attachments/assets/687a52b4-1883-42d5-83dd-2bec049c402b" />

# Wiki

[Powering options](https://github.com/pink0D/CameraBrick/wiki/Powering-options)

[Flashing the firmware](https://github.com/pink0D/CameraBrick/wiki/Flashing-the-firmware)

[Using the CameraBrick](https://github.com/pink0D/CameraBrick/wiki/Using-the-CameraBrick)

# Supported ESP32 boards

| Board | View | Status |
| :--- | :---- |  :---- |
| **Generic ESP32CAM** (a.k.a. AI Thinker) | <img width="150" alt="esp32cam_nobg" src="https://github.com/user-attachments/assets/e5fd0934-cef5-497d-852f-630c7d8b94a2" /> | :white_check_mark: Supported, tested with OV2640 (OV3660 should work as well) |
| [M5Stack AtomS3R-M12](https://docs.m5stack.com/en/core/AtomS3R-M12) | <img width="150" alt="m5-m12" src="https://github.com/user-attachments/assets/63c0360c-79ce-4625-8cb4-d7040f23323f" /> | :white_check_mark: Supported |
| [M5Stack AtomS3R-CAM](https://docs.m5stack.com/en/core/AtomS3R%20Cam) | <img width="150" alt="m5-cam" src="https://github.com/user-attachments/assets/2af9948b-57ea-4870-a004-84ac6bd53c3e" /> | :white_check_mark: Supported (possible lower framerate, low field of view) |
| [M5Stack TimerCamera-F](https://docs.m5stack.com/en/unit/timercam_f) | <img width="150" alt="m5-f" src="https://github.com/user-attachments/assets/4b785262-971f-4030-98fb-e7b1044d9e7e" /> | :white_check_mark: Supported |
| [M5Stack TimerCamera-X](https://docs.m5stack.com/en/unit/timercam_x) | <img width="150" alt="m5-x" src="https://github.com/user-attachments/assets/ac1b68bc-f24a-4f96-a806-5cdd800a9cae" /> | :white_check_mark: Supported (low field of view)|

# Supported Mould King modules

CameraBrick supports MK4 (4-channel) and MK6 (6-channel) power modules controlled with simple Bluetooth LE advertising ([MouldKingino](https://github.com/pink0D/MouldKingino)).

Only MK modules are supported - other clones use different protocols not yet implemented for Arduino 

| Module | View | Status |
| :--- | :--- | :---- |
| **Mould King 4.0 Power Module** <br/> <img width="50" alt="MK logo" src="https://github.com/user-attachments/assets/7d436cb4-5eb3-4f68-9fc1-404786991aca" /> | <img width="150" alt="mk40" src="https://github.com/user-attachments/assets/c3ec0708-9cdb-4308-a025-21dcd572bf07" /> | :white_check_mark: Supported |
| **Mould King 6.0 Power Module** <br/> <img width="50" alt="MK logo" src="https://github.com/user-attachments/assets/7d436cb4-5eb3-4f68-9fc1-404786991aca" /> | <img width="150" alt="mk60" src="https://github.com/user-attachments/assets/b88d586e-67f1-4b0c-a7f2-5ec4de91fed2" /> | :white_check_mark: Supported |


# License & credits
- CameraBrick is open source and licensed under the MIT License

# Contacts
Issues: [CameraBrick issues](https://github.com/pink0D/CameraBrick/issues)

Discussions: [CameraBrick discussions](https://github.com/pink0D/CameraBrick/discussions)

EMail: [pink0D.github@gmail.com](mailto:pink0d.github@gmail.com)
