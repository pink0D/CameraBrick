//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _WIFI_MANAGER_H
#define _WIFI_MANAGER_H

#include <Arduino.h>

#include <list>

#include "ConfigComponent.h"

namespace camerabrick {

    class WiFiManager : public ConfigComponent {

        public:
            WiFiManager() : ConfigComponent("WiFiManager") {};

            bool begin();      
            
            std::string getHostname() {
                return hostname;
            };

            int getRSSI();
            
        private:

            struct WiFiNetwork {
                std::string ssid;
                std::string password;
            };

            std::list<WiFiNetwork> networks;
            WiFiNetwork ap;

            std::string hostname;
            int connectTimeout;

            bool reboot;

            bool apMode = false;

            void loadDefaultSettings() override;
            void applySettings() override;

            bool loadSettingsFromJson(JsonDocument &json) override;
            JsonDocument saveSettingsToJson() override;

            std::string generateDefaultSSID();
            std::string filterString(const std::string& input, char c_min, char c_max, int max_len = 32);
            bool isValidHostname(const std::string& s);
    };
}

namespace camerabrick::comp {
    extern camerabrick::WiFiManager WiFiManager;  
}


#endif