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

#include <NimBLEDevice.h>
#include <MouldKingino.h>

#include "ESP32Camera.h"
#include "ESP32CameraConfig.h"

#include "ConfigComponent.h"

#include <map>

namespace camerabrick::comp {

    class CameraBrick {

        public:
            CameraBrick() {};

            void begin(ESP32Camera::Config cameraConfig);
            void update();

            void registerConfigComponent(std::string name, ConfigComponent* component);
            ConfigComponent* getConfigComponent(std::string name);

        private:

            std::map<std::string, ConfigComponent*> configComponents;

    };
}

extern camerabrick::comp::CameraBrick CameraBrick;  

#endif