//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//


#include "Gamepad.h"

#include <map>

//global instance
namespace camerabrick::comp {
    camerabrick::Gamepad Gamepad;  
}

namespace camerabrick::gamepad {
    GamepadInputMapperImpl GamepadInputMapper;
    GamepadButtonMapperImpl GamepadButtonMapper;
}

namespace camerabrick { 

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

    float GamepadState::getInputValue(gamepad::Input input) {

        switch (input) {

            case gamepad::Input::LeftStickX:
                return leftX();

            case gamepad::Input::LeftStickY:
                return leftY();

            case gamepad::Input::RightStickX:
                return rightX();

            case gamepad::Input::RightStickY:
                return rightY();

            case gamepad::Input::LeftTrigger:
                return leftTrigger();

            case gamepad::Input::RightTrigger:
                return rightTrigger();
        }

        return 0;
    }

    bool GamepadState::getButtonState(gamepad::Button button) {

        int index = static_cast<int>(button);

        if (index > 0)
            return data.buttons & (1 << (index - 1));

        return false;
    }

    bool GamepadState::getButtonClick(gamepad::Button button) {

        static std::map<int, bool> currentClickValues;

        int index = static_cast<int>(button);
        bool newValue = getButtonState(button);

        if (currentClickValues.count(index) == 0)
            currentClickValues[index] = false;

        bool click = false;
        if (newValue) {
            click = !(currentClickValues[index]);
        }
        currentClickValues[index] = newValue;

        return click;        

    }

}