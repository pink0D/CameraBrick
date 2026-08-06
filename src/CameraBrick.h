//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _CAMERA_BRICK_H
#define _CAMERA_BRICK_H

#ifndef BOARD_HAS_PSRAM
    #error PSRAM must be enabled
#endif

#include <Arduino.h>
#include <map>

#include <NimBLEDevice.h>
#include <MouldKingino.h>

#include "ESP32Camera.h"
#include "ConfigComponent.h"
#include "Gamepad.h"
#include "WebServer.h"


namespace camerabrick {

    class Profile {

        public:
            Profile();

            ESP32CameraType cameraType;

            // used for Bluetooth & Wifi radio sync
            // BLE advertisement packets are sent after each MJPEG frame transmission over WiFi completes
            bool syncWithCamera = false;

            // minimum interval between controller state updates            
            int updateDelayMillis = 40;

            // max timeout before controller goes to failsafe mode if no gamepad data is received 
            int gamepadTimeoutMillis = 100;

            // max timeout before controller goes to failsafe mode if no camera frames are captured
            int cameraTimeoutMillis = 200;

            virtual void setup() {};

            virtual void start() {};
            virtual void stop() {};            

            virtual void processGamepad(const GamepadState &gamepad) {};
            virtual void failsafe() {};

            virtual bool isGamepadEnabled() { return false; };

            virtual float getVoltage() { return 0; };
            virtual bool isLowVoltage() { return false; };
    };

    class CameraBrick {

        public:
            CameraBrick() {};

            void setProfile(Profile *profile);
            Profile* getProfile() {
                return profile;
            };

            void begin();
            void update();

            void registerConfigComponent(std::string name, ConfigComponent* component);
            void unregisterConfigComponent(std::string name);
            ConfigComponent* getConfigComponent(std::string name);
            void iterateComponents(const std::function<void(ConfigComponent*)>& callback);

            void cameraSync();

        private:

            std::map<std::string, ConfigComponent*> configComponents;
            camerabrick::Profile *profile = nullptr;

            int64_t lastCameraSync = 0;

            void processGamepad();
    };
}

extern camerabrick::CameraBrick CameraBrick;  

#endif