//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _ESP32CAM_CONFIG_H
#define _ESP32CAM_CONFIG_H

#include <Arduino.h>

#include "ESP32Camera.h"

#define DECLARE_CAMERA_CONFIG(__name__)                   \
    struct __name__ : camerabrick::comp::ESP32Camera::Config {  \
        __name__();                \
    };

namespace camerabrick::config {
    DECLARE_CAMERA_CONFIG(ESP32CAM_OV2640)
    DECLARE_CAMERA_CONFIG(M5STACK_ATOM_S3R_M12_OV3660)
    DECLARE_CAMERA_CONFIG(M5STACK_ATOM_S3R_GC0308)
    DECLARE_CAMERA_CONFIG(M5STACK_TIMER_CAMERA_OV3660)
}

#endif