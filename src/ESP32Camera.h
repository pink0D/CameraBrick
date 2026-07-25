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

namespace camerabrick {

    class ESP32Camera {

        protected:
            ESP32Camera() {};
        
        public:
            static ESP32Camera& instance() {
                static ESP32Camera obj;
                return obj;
            }

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
                int rotateDegrees = 0;
            };

            struct Frame {
                timeval timestamp;
                size_t jpegBufferLength;
                uint8_t *jpegBuffer;
            };     

        public:

            void setConfig(Config &config) {
                this->config = config;
            }                   
            
            bool begin();
            void stop();

            Frame* captureFrame();
            void releaseFrame(Frame *frame);

        private:
            camera_fb_t *fb = nullptr;
            size_t jpg_buf_len = 0;
            uint8_t *jpg_buf = nullptr;

            Config config;
    };
}


#endif