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

namespace camerabrick {

    Profile::Profile() {
        ::CameraBrick.setProfile(this);
    }
}

namespace camerabrick::comp {

    void CameraBrick::setProfile(camerabrick::Profile *profile) {

        if (this->profile != nullptr) {
            Serial.println("WARNING: profile was already set");
        }
        if (profile != nullptr) {
            this->profile = profile;
            Serial.println("CameraBrick profile updated");
        }
    }

    void CameraBrick::begin() {


        esp_chip_info_t chip_info;
        esp_chip_info(&chip_info);
        
        // disable browout check only for ESP32CAM and not for modern ESP32S3
        if (chip_info.model != CHIP_ESP32S3) {
            WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
        }

        Serial.begin(115200);
        Serial.println("CameraBrick startup");
        //Serial.setDebugOutput(true);   
        
        if (profile == nullptr) {
            Serial.println("WARNING: Profile was not set");
            profile = new Profile();
        }

        profile->setup();

        ::camerabrick::ESP32Camera.begin();
        ::camerabrick::WiFiManager.begin();
        ::camerabrick::WebServer.begin();
        ::camerabrick::StreamServer.begin();
        ::camerabrick::Gamepad.begin();

        //::camerabrick::ESP32Camera.blink(5);
    }

    void CameraBrick::registerConfigComponent(std::string name, ConfigComponent* component) {

        configComponents[name] = component;
    }

    ConfigComponent* CameraBrick::getConfigComponent(std::string name) {

        if (configComponents.count(name) > 0)
            return configComponents.at(name);

        return nullptr;
    }

    void CameraBrick::iterateComponents(const std::function<void(ConfigComponent*)>& callback) {

        for (const auto& [_, value] : configComponents) {
            callback(value);
        }
    }

    void CameraBrick::cameraSync() {

        // process gamepad after camera frame was sent over WiFi                
        if (profile->syncWithCamera)
        {                       
            static int64_t time_delay_update = 0;

            int64_t time_now = esp_timer_get_time();

            // limit max updates per seconds if camera FPS gets too high to reduce WiFi/Bluetooth radio switches
            if (time_now > time_delay_update) {

                processGamepad();

                time_delay_update = time_now + profile->updateDelayMillis*1000;
                lastCameraSync = time_now;
            }
        }
    }

    void CameraBrick::update() {


        // go to failsafe mode if camera framerate is low
        if (esp_timer_get_time() > lastCameraSync + profile->cameraTimeoutMillis * 1000) {
            profile->failsafe();
        } 
        else if ((profile->syncWithCamera == false) /*|| (::camerabrick::StreamServer.isActive() == false)*/ ) {
            // process gamepad here if no sync with camera required or camera stream is not active

            processGamepad();    
        } 
        
        vTaskDelay(pdMS_TO_TICKS(profile->updateDelayMillis));
    }

    void CameraBrick::processGamepad() {

        auto state = ::camerabrick::Gamepad.getState();

        int64_t time_now = esp_timer_get_time();
        int64_t gamepadTimeout = state.getTimestamp() + profile->gamepadTimeoutMillis*1000;

        // check if gamepad data was received recently
        if (time_now < gamepadTimeout) {
            profile->processGamepad(state);
        } else {
            profile->failsafe();
        }
    }
}