//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ESP32CameraConfig.h"

namespace camerabrick::config {

    ESP32CAM_OV2640::ESP32CAM_OV2640() {

        sensorType = CameraSensorType::OV2640;
        
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
        espCameraConfig.jpeg_quality = 10;
        
        espCameraConfig.frame_size = FRAMESIZE_QVGA; 
    }

    M5STACK_ATOM_S3R_M12_OV3660::M5STACK_ATOM_S3R_M12_OV3660() {

        sensorType = CameraSensorType::OV3660;
        
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

        //espCameraConfig.sccb_i2c_port = 0;

        espCameraConfig.xclk_freq_hz = 20000000;

        espCameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
        espCameraConfig.fb_count = 2;
        espCameraConfig.grab_mode = CAMERA_GRAB_LATEST;
        
        espCameraConfig.pixel_format = PIXFORMAT_JPEG;
        espCameraConfig.jpeg_quality = 10;
        
        espCameraConfig.frame_size = FRAMESIZE_QVGA; 

        mirrorX = true;
        rotation = 270;
    }

    M5STACK_ATOM_S3R_GC0308::M5STACK_ATOM_S3R_GC0308() {

        sensorType = CameraSensorType::OV3660;
        
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

        //espCameraConfig.sccb_i2c_port = 0;

        espCameraConfig.xclk_freq_hz = 20000000;

        espCameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
        espCameraConfig.fb_count = 2;
        espCameraConfig.grab_mode = CAMERA_GRAB_LATEST;
        
        espCameraConfig.pixel_format = PIXFORMAT_RGB565;
        espCameraConfig.jpeg_quality = 10;
        
        espCameraConfig.frame_size = FRAMESIZE_QVGA; 

        mirrorX = true;
        rotation = 270;
    }

    M5STACK_TIMER_CAMERA_OV3660::M5STACK_TIMER_CAMERA_OV3660() {

        sensorType = CameraSensorType::OV3660;
        
        powerPin = 2;   // actually ledPin
        powerPinValueOn =  HIGH;
        powerPinValueOff = LOW;
        powerDelayMillis = 10;

        ledPin = 2;
        ledPinValueOn = HIGH;
        ledPinValueOff = LOW;

        espCameraConfig.ledc_channel = LEDC_CHANNEL_0;
        espCameraConfig.ledc_timer = LEDC_TIMER_0;

        espCameraConfig.pin_d0 = 32;
        espCameraConfig.pin_d1 = 35;
        espCameraConfig.pin_d2 = 34;
        espCameraConfig.pin_d3 = 5;
        espCameraConfig.pin_d4 = 39;
        espCameraConfig.pin_d5 = 18;
        espCameraConfig.pin_d6 = 36;
        espCameraConfig.pin_d7 = 19;
        espCameraConfig.pin_xclk = 27;
        espCameraConfig.pin_pclk = 21;
        espCameraConfig.pin_vsync = 22;
        espCameraConfig.pin_href = 26;
        espCameraConfig.pin_sccb_sda = 25;
        espCameraConfig.pin_sccb_scl = 23;
        espCameraConfig.pin_pwdn = -1;
        espCameraConfig.pin_reset = 15;

        //espCameraConfig.sccb_i2c_port = 0;

        espCameraConfig.xclk_freq_hz = 20000000;

        espCameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
        espCameraConfig.fb_count = 2;
        espCameraConfig.grab_mode = CAMERA_GRAB_LATEST;
        
        espCameraConfig.pixel_format = PIXFORMAT_JPEG;
        espCameraConfig.jpeg_quality = 10;
        
        espCameraConfig.frame_size = FRAMESIZE_QVGA; 

        mirrorY = true;
    }

}