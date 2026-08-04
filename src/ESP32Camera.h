//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _ESP32CAM_H
#define _ESP32CAM_H

#include <Arduino.h>

#include <esp_camera.h>

#include "ConfigComponent.h"

namespace camerabrick {

    enum class ESP32CameraType {ESP32CAM_OV2640, M5STACK_ATOM_S3R_M12_OV3660, M5STACK_ATOM_S3R_GC0308, M5STACK_TIMER_CAMERA_OV3660};

    class ESP32Camera : public ConfigComponent {

        public:
            ESP32Camera() : ConfigComponent("ESP32Camera") {};
        
        public:

            struct Config {

                camera_config_t espCameraConfig;

                int powerPin = -1;
                int powerPinValueOn;
                int powerPinValueOff;
                int powerDelayMillis = 500;

                int ledPin = -1;
                int ledPinValueOn;
                int ledPinValueOff;

                bool mirrorY = false;
                bool mirrorX = false;
                int rotation = 0;
            };

            struct Frame {
                timeval timestamp;
                size_t jpegBufferLength;
                uint8_t *jpegBuffer;
            };     

        public:

            void begin();

            void blink(int count);

            bool startCapture();
            void stopCapture();

            Frame* captureFrame();
            void releaseFrame(Frame *frame);

            int getRotation() {
                return config.rotation;
            };

        private:
            camera_fb_t *fb = nullptr;
            size_t jpg_buf_len = 0;
            uint8_t *jpg_buf = nullptr;

            Config config;
            Frame frame;

            bool loadSettingsFromJson(JsonDocument &json) override;
            JsonDocument saveSettingsToJson() override;

    };
}

namespace camerabrick::comp {
    extern camerabrick::ESP32Camera ESP32Camera;  
}

#endif