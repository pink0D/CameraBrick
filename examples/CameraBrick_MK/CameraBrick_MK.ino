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

const auto CAMERA_TYPE = camerabrick::ESP32CameraType::ESP32CAM_OV2640;

const int MouldKingMaxChannels = 6;

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

class MouldKingProfile: public camerabrick::Profile, camerabrick::ConfigComponent {

    public:
        MouldKingProfile() : camerabrick::ConfigComponent("MouldKingProfile") {};

        ChannelConfig channelConfig[MouldKingMaxChannels];

        MouldKingModuleType moduleType;
        MouldKing40 mk;

        void setup() override {

            ConfigComponent::begin();

            cameraType = CAMERA_TYPE;

            syncWithCamera = true;

            NimBLEDevice::init("");
            NimBLEDevice::setPower(-12, NimBLETxPowerType::Advertise);

            esp_coex_preference_set(ESP_COEX_PREFER_WIFI);
        }

        void start() override {
            // reset adv count!!!
            mk.connect();
        }

        void stop() override {
            mk.disconnect();
        }

        void processGamepad(const camerabrick::GamepadState &gamepad) override {

            mk.updateMotorOutput(MOTOR_A, gamepad.leftY() );
            mk.applyUpdates(30, true);
        }

        void failsafe() override {

            mk.updateMotorOutput(MOTOR_A, 0);
            mk.applyUpdates(30, true);
        }

        bool isGamepadEnabled() override {

            return moduleType != MouldKingModuleType::None;
        }

        void applySettings() override {
                // reset adv count!!!
        };

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
