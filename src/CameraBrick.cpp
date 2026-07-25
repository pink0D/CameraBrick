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

#include "soc/soc.h"             
#include "soc/rtc_cntl_reg.h"    


//global instance
camerabrick::CameraBrick CameraBrick;  

namespace camerabrick {

    void CameraBrick::begin(ESP32Camera::Config cameraConfig) {

        WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

        Serial.begin(115200);
        //Serial.setDebugOutput(true);

        //NimBLEDevice::init("");

        ESP32Camera::instance().setConfig(cameraConfig);

        WiFiManager::instance().begin();

        StreamServer::instance().begin();
    }

    void CameraBrick::update() {
        delay(1000);
    }
}