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

#include <map>

namespace camerabrick {

    template <class EnumClass>
    class EnumMapper {
        public:
            EnumMapper() {};

            void checkInit() {
                static bool done = false;
                if (!done) {
                    init();
                    done = true;
                }
            }

            std::string mapEnumToString(EnumClass e) {

                checkInit();

                if (m.count(e) > 0)
                    return m.at(e);

                return defaultString;
            };

            EnumClass mapStringToEnum(std::string s) {

                checkInit();

                auto it = std::find_if(m.begin(), m.end(), 
                    [&s](const std::pair<EnumClass, std::string>& pair) {
                        return pair.second == s;
                    });

                if (it != m.end()) 
                    return it->first;

                return defaultEnum;
            };

        protected:
            virtual void init() {};

            void addMap(EnumClass e, std::string s) {
                m[e] = s;
            };

            void setDefaultEnum(EnumClass e) {
                defaultEnum = e;
            };

            void setDefaultString(std::string s) {
                defaultString = s;
            };

        private:
            std::map<EnumClass,std::string> m;
            std::string defaultString;
            EnumClass defaultEnum;
    };

    class ConfigComponent {

        public:
            ConfigComponent(std::string name) {
                this->name = name;
            };

            void begin();
            
            virtual void loadDefaultSettings() {};
            virtual void applySettings() {};

            virtual bool loadSettingsFromJson(JsonDocument &json) { return false; };
            virtual JsonDocument saveSettingsToJson() { return JsonDocument(); };

            void loadSettingsFromStorage();
            void saveSettingsToStorage();

        private:
            std::string name;

            String getConfigPath();

    };
}

#endif