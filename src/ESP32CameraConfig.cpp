//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ESP32CameraConfig.h"

ESP32CAM_OV2640::ESP32CAM_OV2640(framesize_t _frame_size) {
    
    powerPin = 32;
    powerPinValueOn =  LOW;
    powerPinValueOff = HIGH;
    powerDelayMillis = 500;

    ledPin = 4;
    ledPinValueOn = HIGH;
    ledPinValueOff = LOW;

    espCameraConfig.ledc_channel = LEDC_CHANNEL_0;
    espCameraConfig.ledc_timer = LEDC_TIMER_0;

    espCameraConfig.pin_d0 = 5;
    espCameraConfig.pin_d1 = 18;
    espCameraConfig.pin_d2 = 19;
    espCameraConfig.pin_d3 = 21;
    espCameraConfig.pin_d4 = 36;
    espCameraConfig.pin_d5 = 39;
    espCameraConfig.pin_d6 = 34;
    espCameraConfig.pin_d7 = 35;
    espCameraConfig.pin_xclk = 0;
    espCameraConfig.pin_pclk = 22;
    espCameraConfig.pin_vsync = 25;
    espCameraConfig.pin_href = 23;
    espCameraConfig.pin_sccb_sda = 26;
    espCameraConfig.pin_sccb_scl = 27;
    espCameraConfig.pin_pwdn = -1;
    espCameraConfig.pin_reset = -1;

    espCameraConfig.xclk_freq_hz = 20000000;

    espCameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
    espCameraConfig.fb_count = 2;
    espCameraConfig.grab_mode = CAMERA_GRAB_LATEST;
    
    espCameraConfig.pixel_format = PIXFORMAT_JPEG;
    espCameraConfig.jpeg_quality = 12;
    
    espCameraConfig.frame_size = _frame_size; 
}

M5STACK_ATOM_S3R_M12_OV3660::M5STACK_ATOM_S3R_M12_OV3660(framesize_t _frame_size) {
    
    powerPin = 18;
    powerPinValueOn =  LOW;
    powerPinValueOff = HIGH;
    powerDelayMillis = 500;

    espCameraConfig.ledc_channel = LEDC_CHANNEL_0;
    espCameraConfig.ledc_timer = LEDC_TIMER_0;

    espCameraConfig.pin_d0 = 3;
    espCameraConfig.pin_d1 = 42;
    espCameraConfig.pin_d2 = 46;
    espCameraConfig.pin_d3 = 48;
    espCameraConfig.pin_d4 = 4;
    espCameraConfig.pin_d5 = 17;
    espCameraConfig.pin_d6 = 11;
    espCameraConfig.pin_d7 = 13;
    espCameraConfig.pin_xclk = 21;
    espCameraConfig.pin_pclk = 40;
    espCameraConfig.pin_vsync = 10;
    espCameraConfig.pin_href = 14;
    espCameraConfig.pin_sccb_sda = 12;
    espCameraConfig.pin_sccb_scl = 9;
    espCameraConfig.pin_pwdn = -1;
    espCameraConfig.pin_reset = -1;

    espCameraConfig.sccb_i2c_port = 0;

    espCameraConfig.xclk_freq_hz = 20000000;

    espCameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
    espCameraConfig.fb_count = 2;
    espCameraConfig.grab_mode = CAMERA_GRAB_LATEST;
    
    espCameraConfig.pixel_format = PIXFORMAT_JPEG;
    espCameraConfig.jpeg_quality = 12;
    
    espCameraConfig.frame_size = _frame_size; 
}