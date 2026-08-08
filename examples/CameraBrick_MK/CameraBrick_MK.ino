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

#include "app_mk_files.h"

const auto CAMERA_TYPE = camerabrick::ESP32CameraType::ESP32CAM_GENERIC;


class MouldKingProfile: public camerabrick::Profile, camerabrick::ConfigComponent {

    enum class MouldKingModuleType {None, MK40, MK60};

    class : public camerabrick::EnumMapper<MouldKingModuleType> {
            void init() override {
                addMap(MouldKingModuleType::None, "");
                addMap(MouldKingModuleType::MK40, "MK40");
                addMap(MouldKingModuleType::MK60, "MK60");

                setDefaultEnum(MouldKingModuleType::None);
                setDefaultString("");
            };
    } MouldKingModuleTypeMapper;

    struct ChannelConfig {
        camerabrick::gamepad::Input input;
        camerabrick::gamepad::Button button;
        bool invert;
    };

    public:
        MouldKingProfile() : camerabrick::ConfigComponent("MouldKingProfile") {};

        static const int advertisingTimeout = 30; 

        static const int MouldKingMaxChannels = 6;
        ChannelConfig channelConfig[MouldKingMaxChannels];
        bool channelToggle[MouldKingMaxChannels];

        MouldKingModuleType moduleType;
        IMKModule *mk = nullptr;


        void setup() override {

            ConfigComponent::begin();

            cameraType = CAMERA_TYPE;

            // controller updates must be synced with camera frame capture
            // ESP32 shares same radio for WiFi and Bluetooth and performance degrades if they are used together
            // So BLE advertisements should be sent after image transmission over WiFi is complete
            syncWithCamera = true;

            NimBLEDevice::init("");
            NimBLEDevice::setPower(-12, NimBLETxPowerType::Advertise); 
            // reduce BLE power since ESP32CAM needs lot of power for camera and WiFi

            esp_coex_preference_set(ESP_COEX_PREFER_WIFI); // prefer WiFi over Bluetooth to keep framerate stable

            // add files for web app
            camerabrick::comp::WebServer.addWebFiles(app_mk_web_data);
        }

        void start() override {

            if (mk != nullptr)
                stop();

            switch (moduleType) {

                case MouldKingModuleType::None:
                    return;

                case MouldKingModuleType::MK40:
                    mk = new MouldKing40();
                    break;

                case MouldKingModuleType::MK60:
                    mk = new MouldKing60();
                    break;
            }

            for (int i=0; i<mk->getChannelCount(); i++) {
                channelToggle[i] = false;
            }

            mk->connect(); // sync connect, waits 1 sec

            this->failsafe(); // start in failsafe state
        }

        void stop() override {

            if (mk != nullptr) {

                this->failsafe(); // set failsafe state before disconnecting
                vTaskDelay(pdMS_TO_TICKS(advertisingTimeout * 4)); // time to advertise state
                
                mk->disconnect();

                delete mk;
                mk = nullptr;
            }
        }

        void processGamepad(const camerabrick::GamepadState &gamepad) override {

            if (mk == nullptr)
                return;

            for (int i=0; i<mk->getChannelCount(); i++) {

                float value = gamepad.getInputValue(channelConfig[i].input); // will return 0 if input is not set

                if (channelConfig[i].button != camerabrick::gamepad::Button::NullButton) {

                    if (gamepad.getButtonClick(channelConfig[i].button)) { // will return false is button is not set
                        channelToggle[i] = !channelToggle[i];
                    }

                    value = channelToggle[i]  ? 1.0 : 0.0;
                }

                if (channelConfig[i].invert) {
                    value = -value;
                }

                mk->updateMotorOutput(i, value);
            }                            

            // advertise immediately with limited timeout so it does not affect sending next camera frame over WiFi
            mk->applyUpdates(advertisingTimeout, true);
        }

        void failsafe() override {

            if (mk == nullptr)
                return;

            // do not use resetChannels because it starts advertisement with no timeout
            for (int i=0; i<mk->getChannelCount(); i++) {
                mk->updateMotorOutput(i, 0);
                channelToggle[i] = false;
            }

            // advertise immediately with limited timeout 
            mk->applyUpdates(advertisingTimeout, true);
        }

        bool isGamepadEnabled() override {

            return moduleType != MouldKingModuleType::None;
        }

        void loadDefaultSettings() override {

            moduleType = MouldKingModuleType::None;

            for (int i=0; i<MouldKingMaxChannels; i++) {
                channelConfig[i].input = camerabrick::gamepad::Input::NullInput;
                channelConfig[i].button = camerabrick::gamepad::Button::NullButton;
                channelConfig[i].invert = false;
            }

        };

        bool loadSettingsFromJson(JsonDocument &json) override {

            moduleType = MouldKingModuleTypeMapper.mapStringToEnum(json["moduleType"]);

            for (int i=0; i<MouldKingMaxChannels; i++) {

                channelConfig[i].input = camerabrick::gamepad::GamepadInputMapper.mapStringToEnum(json["channels"][i]["input"]);
                channelConfig[i].button = camerabrick::gamepad::GamepadButtonMapper.mapStringToEnum(json["channels"][i]["button"]);
                channelConfig[i].invert = json["channels"][i]["invert"];
            }

            return true;
        };

        JsonDocument saveSettingsToJson() override {

            JsonDocument json;

            json["moduleType"] = MouldKingModuleTypeMapper.mapEnumToString(moduleType);

            for (int i=0; i<MouldKingMaxChannels; i++) {

                JsonDocument jsonChannel;

                jsonChannel["input"] = camerabrick::gamepad::GamepadInputMapper.mapEnumToString(channelConfig[i].input);
                jsonChannel["button"] = camerabrick::gamepad::GamepadButtonMapper.mapEnumToString(channelConfig[i].button);
                jsonChannel["invert"] = channelConfig[i].invert;

                json["channels"][i] = jsonChannel;
            }

            return json;
        };

} profile;

void setup() {
    CameraBrick.begin();
}

void loop() {
    CameraBrick.update();
}
