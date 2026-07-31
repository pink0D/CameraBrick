//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "CameraBrick.h"
#include "WiFiManager.h"
#include "StreamServer.h"
#include "WebServer.h"

#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
#include <esp_chip_info.h>


//global instance
camerabrick::comp::CameraBrick CameraBrick;  

namespace camerabrick::comp {

    void CameraBrick::begin(ESP32Camera::Config cameraConfig) {


        esp_chip_info_t chip_info;
        esp_chip_info(&chip_info);
        
        // disable browout check only for ESP32CAM and not for modern ESP32S3
        if (chip_info.model != CHIP_ESP32S3) {
            WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
        }

        Serial.begin(115200);
        //Serial.setDebugOutput(true);        

        ::camerabrick::ESP32Camera.setConfig(cameraConfig);

        ::camerabrick::WiFiManager.begin();

        ::camerabrick::WebServer.begin();

        ::camerabrick::StreamServer.begin();
    }

    void CameraBrick::update() {
        delay(1000);
    }

    void CameraBrick::registerConfigComponent(std::string name, ConfigComponent* component) {
        
        configComponents[name] = component;
    }

    ConfigComponent* CameraBrick::getConfigComponent(std::string name) {

        if (configComponents.count(name) > 0)
            return configComponents.at(name);

        return nullptr;
    }

}