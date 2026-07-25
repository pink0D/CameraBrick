//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "WiFiManager.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include "secrets.h"

// WiFi credentials are set in secrets.h
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *hostname = "fpvbrick";

namespace camerabrick {

    bool WiFiManager::begin() {

        WiFi.setHostname(hostname);
        WiFi.begin(ssid, password);
        WiFi.setSleep(false);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.println("WiFi connected");

        if (!MDNS.begin(hostname)) {
            Serial.println("Error setting up MDNS responder!");
        }
        else {
            MDNS.addService("http", "tcp", 80);
        }

        Serial.println("mDNS responder started");

        return true;
    }
    
}