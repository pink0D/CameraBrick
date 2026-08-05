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

#include "ConfigComponent.h"

namespace camerabrick::gamepad {

    enum class Input {NullInput, 
        LeftStickX, LeftStickY, 
        RightStickX, RightStickY, 
        LeftTrigger, RightTrigger};

    enum class Button {NullButton, 
        Cross, Circle, Square, Triangle, 
        L1, R1, L2, R2, 
        Share, Options, 
        L3, R3, 
        DPadUp, DPadDown, DPadLeft, DPadRight};

    class GamepadInputMapperImpl: public camerabrick::EnumMapper<gamepad::Input> {
            void init() override {
                addMap(gamepad::Input::NullInput, "");
                addMap(gamepad::Input::LeftStickX, "LeftStickX");
                addMap(gamepad::Input::LeftStickY, "LeftStickY");
                addMap(gamepad::Input::RightStickX, "RightStickX");
                addMap(gamepad::Input::RightStickY, "RightStickY");
                addMap(gamepad::Input::LeftTrigger, "LeftTrigger");
                addMap(gamepad::Input::RightTrigger, "RightTrigger");

                setDefaultEnum(gamepad::Input::NullInput);
                setDefaultString("");
            };
    };    

    extern GamepadInputMapperImpl GamepadInputMapper;

    class GamepadButtonMapperImpl: public camerabrick::EnumMapper<gamepad::Button> {
            void init() override {
                addMap(gamepad::Button::NullButton, "");
                addMap(gamepad::Button::Cross, "Cross");
                addMap(gamepad::Button::Circle, "Circle");
                addMap(gamepad::Button::Square, "Square");
                addMap(gamepad::Button::Triangle, "Triangle");
                addMap(gamepad::Button::L1, "L1");
                addMap(gamepad::Button::R1, "R1");
                addMap(gamepad::Button::L2, "L2");
                addMap(gamepad::Button::R2, "R2");
                addMap(gamepad::Button::Share, "Share");
                addMap(gamepad::Button::Options, "Options");
                addMap(gamepad::Button::L3, "L3");
                addMap(gamepad::Button::R3, "R3");
                addMap(gamepad::Button::DPadUp, "DPadUp");
                addMap(gamepad::Button::DPadDown, "DPadDown");
                addMap(gamepad::Button::DPadLeft, "DPadLeft");
                addMap(gamepad::Button::DPadRight, "DPadRight");

                setDefaultEnum(gamepad::Button::NullButton);
                setDefaultString("");
            };
    };  

    extern GamepadButtonMapperImpl GamepadButtonMapper;

}

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

            GamepadState(gamepad_raw_data &rawData, int64_t timestamp) {
                this->data = rawData;
                this->timestamp = timestamp;
            };

            GamepadState(const GamepadState& s) {
                this->data = s.data;
                this->timestamp = s.timestamp;
            };

            GamepadState& operator=(const GamepadState &s) {
                this->data = s.data;
                this->timestamp = s.timestamp;
                return *this;
            };

            float leftX() const { return data.LX; };
            float leftY() const { return data.LY; };
            float rightX() const { return data.RX; };
            float rightY() const { return data.RY; };
            float leftTrigger() const { return data.LT; };
            float rightTrigger() const { return data.RT; };

            float getInputValue(gamepad::Input input) const;
            bool getButtonState(gamepad::Button button) const;
            bool getButtonClick(gamepad::Button button) const;

            int64_t getTimestamp() const { return timestamp; };

        private:
            gamepad_raw_data data;
            int64_t timestamp = 0;
                    
    };

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

namespace camerabrick::comp {
    extern camerabrick::Gamepad Gamepad;  
}

#endif