//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ConfigComponent.h"
#include "CameraBrick.h"

#include <FS.h>
#include <LittleFS.h>


namespace camerabrick {

    String ConfigComponent::getConfigPath() {
        return String("/") + String(name.c_str()) + String(".json");
    }

    void ConfigComponent::begin() {
        loadDefaultSettings();
        loadSettingsFromStorage();
        applySettings(); 
        
        ::CameraBrick.registerConfigComponent(name, this);
    };

    void ConfigComponent::loadSettingsFromStorage() {

        static bool fs_init_done = false;

        if (!fs_init_done) {
            if (!LittleFS.begin(true)) {
                Serial.println("WARNING: LittleFS begin Failed");
                return;
            }
            fs_init_done = true;
        }

        File file = LittleFS.open(getConfigPath(), FILE_READ);
        if (!file) {
            Serial.print("WARNING: Failed to open config file for reading: ");
            Serial.println(getConfigPath());
            return;
        }

        JsonDocument json; 

        DeserializationError error = deserializeJson(json, file);

        file.close();

        if (error) {
            Serial.println(getConfigPath());
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        loadSettingsFromJson(json);
    };
    
    void ConfigComponent::saveSettingsToStorage() {
        File file = LittleFS.open(getConfigPath(), FILE_WRITE);
        if (!file) {
            Serial.print("WARNING: Failed to open config file for writing: ");
            Serial.println(getConfigPath());
            return;
        }

        if (serializeJson(saveSettingsToJson(), file) == 0) {
            Serial.print("Failed to write to config file: ");
            Serial.println(getConfigPath());
        }

        file.close();
    };

}