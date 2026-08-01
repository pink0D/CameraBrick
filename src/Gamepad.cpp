//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//


#include "Gamepad.h"

//global instance
namespace camerabrick {
    camerabrick::comp::Gamepad Gamepad;  
}

namespace camerabrick::comp {

    void Gamepad::begin() {
        mutex = xSemaphoreCreateMutex();
    }

    void Gamepad::updateData(gamepad_raw_data &rawData) {

        xSemaphoreTake(mutex, portMAX_DELAY);

        this->state = GamepadState(rawData, esp_timer_get_time());

        xSemaphoreGive(mutex);
    }

    GamepadState Gamepad::getState() {

        xSemaphoreTake(mutex, portMAX_DELAY);

        GamepadState s = this->state; // create a copy of current state

        xSemaphoreGive(mutex);

        return s; // NRVO
    }

}