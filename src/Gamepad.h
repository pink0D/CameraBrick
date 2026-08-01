//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include <Arduino.h>
#include <freertos/semphr.h>

namespace camerabrick {

    struct __attribute__((packed)) gamepad_raw_data {
        double timestamp = 0;
        float LX = 0;
        float LY = 0;
        float RX = 0;
        float RY = 0;
        float LT = 0;
        float RT = 0;
        uint16_t buttons = 0;      
    };        

    class GamepadState {

        public:
            GamepadState() {};

            GamepadState(gamepad_raw_data &rawData, int64_t timeUpdate) {
                this->data = rawData;
                this->timeUpdate = timeUpdate;
            };

            GamepadState(const GamepadState& s) {
                this->data = s.data;
                this->timeUpdate = s.timeUpdate;
            };

            GamepadState& operator=(const GamepadState &s) {
                this->data = s.data;
                this->timeUpdate = s.timeUpdate;
                return *this;
            };

            float leftX() const { return data.LX; };
            float leftY() const { return data.LY; };
            float rightX() const { return data.RX; };
            float rightY() const { return data.RY; };
            float leftTrigger() const { return data.LT; };
            float rightTrigger() const { return data.RT; };

        private:
            gamepad_raw_data data;
            int64_t timeUpdate = 0;
                    
    };
}

namespace camerabrick::comp {

    class Gamepad {

        public:
            Gamepad() {};

            void begin();
            void updateData(gamepad_raw_data &rawData);
            GamepadState getState();

        private:
            GamepadState state;
            SemaphoreHandle_t mutex = nullptr;
                    
    };
}

namespace camerabrick {
    extern comp::Gamepad Gamepad;  
}

#endif