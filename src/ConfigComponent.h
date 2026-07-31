//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _CONFIG_COMPONENT_H
#define _CONFIG_COMPONENT_H


#include <Arduino.h>
#include <ArduinoJson.h>

namespace camerabrick::comp {

    class ConfigComponent {

        public:
            ConfigComponent() {};

            void begin() {
                loadDefaultSettings();
                loadSettingsFromStorage();
                applySettings();                
            };
            
            virtual void loadDefaultSettings() {};
            virtual void applySettings() {};

            virtual bool loadSettingsFromJson(JsonDocument &json) { return false; };
            virtual JsonDocument saveSettingsToJson() { return JsonDocument(); };

            void loadSettingsFromStorage() {};
            void saveSettingsToStorage() {};

    };
}

#endif