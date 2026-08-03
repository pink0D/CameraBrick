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
#include <esp_mac.h>
#include <esp_wifi.h>

#include <ESPmDNS.h>

#if __has_include("secrets.h")

#define WIFI_DEBUG

// WiFi credentials for debug are set in secrets.h
#include "secrets.h"

#endif

//global instance
namespace camerabrick {
    camerabrick::comp::WiFiManager WiFiManager;  
}

namespace camerabrick::comp {

    bool WiFiManager::begin() {

        ConfigComponent::begin();

        // add debug network
        #ifdef WIFI_DEBUG    
            WiFiNetwork network;
            network.ssid = WIFI_SSID;
            network.password = WIFI_PASSWORD;
            networks.push_back(network);            
        #endif

        WiFi.setHostname(hostname.c_str());

        for (const auto& network : networks) {

            Serial.print("Connecting to ");
            Serial.print(network.ssid.c_str());

            int timeout = this->connectTimeout;

            WiFi.begin(network.ssid.c_str(), network.password.c_str());
            WiFi.setSleep(false);

            while (timeout-- > 0) {

                vTaskDelay(pdMS_TO_TICKS(1000));
                Serial.print(" . ");
                
                if (WiFi.status() == WL_CONNECTED)
                    break;
            }

            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("Connected");
                Serial.print("Local IP: ");
                Serial.println(WiFi.localIP());
                break;
            }
            else {
                // disconnect before trying next network without completely turning WiFi off
                WiFi.disconnect(false, true, 1000);
                Serial.println("Timeout");
            }
        }

        // Start in AP mode if no WiFi networks were available
        if (WiFi.status() != WL_CONNECTED) {

            WiFi.mode(WIFI_AP);
            this->apMode = true;

            if (WiFi.softAP(ap.ssid.c_str(), ap.password.c_str())) {

                WiFi.setSleep(false);
                Serial.print("Started WiFi in AP mode with SSID ");
                Serial.println(ap.ssid.c_str());
                Serial.print("AP IP: ");
                Serial.println(WiFi.softAPIP());

            } else {
                Serial.println("Error starting WiFi");
            }
        }

        if (!MDNS.begin(hostname.c_str())) {
            Serial.println("Error setting up MDNS responder!");
        }
        else {
            MDNS.addService("http", "tcp", 80);
        }

        Serial.println("mDNS responder started");

        Serial.print("Hostname: ");
        Serial.println(hostname.c_str());

        return true;
    }

    int WiFiManager::getRSSI() {

        int rssi = -100;

        if (apMode) {

            wifi_sta_list_t wifi_sta_list;
            memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));

            if (esp_wifi_ap_get_sta_list(&wifi_sta_list) == ESP_OK) {
                if (wifi_sta_list.num > 0) {
                    rssi = wifi_sta_list.sta[0].rssi;
                }
            }     

        } else {
            rssi = WiFi.RSSI();
        }

        return rssi;
    }

    void WiFiManager::loadDefaultSettings() {       

        ap.ssid = generateDefaultSSID();
        ap.password = "";

        hostname = "fpv";
        connectTimeout = 10;

        reboot = false;
    }

    void WiFiManager::applySettings() {

        if (reboot) {
            ESP.restart();
        }
    }

    std::string WiFiManager::generateDefaultSSID() {
        uint8_t wifi_mac[6];
        esp_read_mac(wifi_mac, ESP_MAC_WIFI_SOFTAP);

        char ssid[32];
        sprintf(ssid, "CameraBrick_%02X%02X%02X", wifi_mac[3], wifi_mac[4], wifi_mac[5]);

        return ssid;
    }

    std::string WiFiManager::filterString(const std::string& input, char c_min, char c_max, int max_len) {
        std::string result;
        for (char c : input) {
            if (c >= c_min && c <= c_max)  {
                result.push_back(c);
            }
        }
        if (result.length() > max_len) {
            result.resize(max_len);
        }
        return result;
    }

    bool WiFiManager::isValidHostname(const std::string& s) {
        if (s.empty() || s.length() > 63) return false;
        if (s.front() == '-' || s.back() == '-') return false;

        for (char c : s) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-') {
                return false;
            }
        }
        return true;
    }

    bool WiFiManager::loadSettingsFromJson(JsonDocument &json) {

        bool configValid = true;

        std::string new_hostname = (std::string)json["hostname"];
        std::string new_ap_ssid = json["ap"]["ssid"];
        std::string new_ap_password = json["ap"]["password"];

        int new_connectTimeout = json["timeout"];
        if (new_connectTimeout == 0) {
            new_connectTimeout = 10;
        }

        configValid &= isValidHostname(new_hostname);
        configValid &= filterString(new_ap_ssid, 32, 126, 32) == new_ap_ssid;
        configValid &= filterString(new_ap_password, 32, 126, 32) == new_ap_password;        

        networks.clear();

        for (int i=0; i<json["networks"].size(); i++) {
            WiFiNetwork network;
            network.ssid = (std::string)json["networks"][i]["ssid"];
            network.password = (std::string)json["networks"][i]["password"];

            // save only valid WiFi credentials
            if ( (filterString(network.ssid, 32, 126, 32) == network.ssid) 
                && (filterString(network.password, 32, 126, 32) == network.password) ) {
                networks.push_back(network);
            }
        }

        reboot = json["reboot"]; // used to reboot the device in applySettings(), this value must not be saved to JSON!

        if (configValid) {
            hostname = new_hostname;
            connectTimeout = new_connectTimeout;
            ap.ssid = new_ap_ssid;
            ap.password = new_ap_password;
        }

        return configValid;
    }

    JsonDocument WiFiManager::saveSettingsToJson() {

        JsonDocument json;

        json["hostname"] = hostname;
        json["timeout"] = connectTimeout;

        JsonDocument ap;
        ap["ssid"] = this->ap.ssid;
        ap["password"] = this->ap.password;

        json["ap"] = ap;

        int n=0;
        for (const auto& network : networks) {
            JsonDocument nw;
            nw["ssid"] = network.ssid;
            nw["password"] = network.password;
            json["networks"][n++] = nw;
        }

        return json;
    }
    

}