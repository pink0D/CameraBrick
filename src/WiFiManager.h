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

namespace camerabrick {

    class WiFiManager {

        public:
            static WiFiManager& instance() {
                static WiFiManager obj;
                return obj;
            }

            bool begin();

        private:
            WiFiManager() {};

    };
}


#endif