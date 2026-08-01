//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//


// Arduino IDE sketch settings:
//
// PSRAM: enabled 
// Partition scheme: HUGE APP

#include <CameraBrick.h>
#include <MouldKingino.h>
#include <esp_coexist.h>


MouldKing40 mk;

class : public camerabrick::Profile {

    void setup() {

        cameraType = camerabrick::ESP32CameraType::ESP32CAM_OV2640;

        syncWithCamera = true;

        NimBLEDevice::init("");
        NimBLEDevice::setPower(-12, NimBLETxPowerType::Advertise);

        esp_coex_preference_set(ESP_COEX_PREFER_WIFI);

        mk.connectAsync(); 
    }

    void processGamepad(const camerabrick::GamepadState &gamepad) {

        mk.updateMotorOutput(MOTOR_A, gamepad.leftY() );
        mk.applyUpdates(30, true);
    }

    virtual void failsafe() {

        mk.updateMotorOutput(MOTOR_A, 0);
        mk.applyUpdates(30, true);
    }


} MKProfile;

void setup() {
    CameraBrick.begin();
}

void loop() {
    CameraBrick.update();
}
