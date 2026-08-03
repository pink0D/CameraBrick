//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "WebServer.h"

#include <esp_http_server.h>

#include "StreamServer.h"
#include "ConfigComponent.h"
#include "CameraBrick.h"
#include "Gamepad.h"
#include "WiFiManager.h"

#include "../extras/app_camera_files.h"

//global instance
namespace camerabrick {
    camerabrick::comp::WebServer WebServer;  
}

namespace camerabrick::comp {    

    bool WebServer::begin() {

        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.max_uri_handlers = 16;
        config.uri_match_fn = httpd_uri_match_wildcard;

        httpd_uri_t stream_uri = {
            .uri = "/ws",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->websocket_handler(req);
            },
            .user_ctx = this,
            .is_websocket = true
        };  

        httpd_uri_t root_config_uri = {
            .uri = "/config",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->root_config_handler(req);
            },
            .user_ctx = this,
        };  

        httpd_uri_t component_config_get_uri = {
            .uri = "/config/*",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->component_config_handler_get(req);
            },
            .user_ctx = this,
        };  

        httpd_uri_t component_config_post_uri = {
            .uri = "/config/*",
            .method = HTTP_POST,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->component_config_handler_post(req);
            },
            .user_ctx = this,
        };  

        httpd_uri_t files_uri = {
            .uri = "/*",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->file_handler(req);
            },
            .user_ctx = this,
        };  

        config.server_port = 80;
        config.ctrl_port = 30080;

        Serial.print("Starting web server on port ");
        Serial.println(config.server_port);

        
        if (httpd_start(&web_httpd, &config) == ESP_OK) {
            httpd_register_uri_handler(web_httpd, &stream_uri);
            httpd_register_uri_handler(web_httpd, &root_config_uri);
            httpd_register_uri_handler(web_httpd, &component_config_get_uri);
            httpd_register_uri_handler(web_httpd, &component_config_post_uri);
            httpd_register_uri_handler(web_httpd, &files_uri);
        }
        

        Serial.println("Web server started");

        return true;
    }

    esp_err_t WebServer::httpd_resp_send_json_chunk(httpd_req_t *req, JsonDocument &json) {

        size_t jsonLength = measureJsonPretty(json);
        char* buffer = new char[jsonLength + 1];

        serializeJsonPretty(json, buffer, jsonLength + 1);

        esp_err_t res = httpd_resp_send_chunk(req, buffer, jsonLength);
        httpd_resp_send_chunk(req, nullptr, 0);

        delete [] buffer;

        return res;
    }

    esp_err_t WebServer::websocket_handler(httpd_req_t *req) {

        esp_err_t res = ESP_OK;

        httpd_ws_frame_t ws_pkt;
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

        // get frame size
        res = httpd_ws_recv_frame(req, &ws_pkt, 0);
        if (res != ESP_OK) {
            Serial.printf("httpd_ws_recv_frame failed to get frame len with %d", res);
            Serial.println();
            return res;
        }

        if (ws_pkt.len > 127) {
            Serial.println("WS Incoming packet too long");
            return ESP_FAIL;
        }

        uint8_t buf[128] = {0};
        ws_pkt.payload = buf;

        res = httpd_ws_recv_frame(req, &ws_pkt, sizeof(buf));
        if (res != ESP_OK) {
            Serial.printf("httpd_ws_recv_frame failed with %d", res);
            Serial.println();
            return res;
        }

        
        if (ws_pkt.type != HTTPD_WS_TYPE_BINARY) {
            Serial.println("WS Incoming packet is not binary");
            //return ESP_FAIL;
        }

        if (strncmp((const char*)buf,"ping", 4) == 0) {
            
            const char* json_fmt = "{\"rssi\":%d, \"ping\":%s, \"fps\":%d, \"voltage\":%s, \"low_voltage_flag\":%s}";
            char json[256];

            int rssi = ::camerabrick::WiFiManager.getRSSI();
            
            sprintf(json, json_fmt, 
                rssi, 
                &buf[5], // timestamp sent thru ws to calculate ping in browser
                ::camerabrick::StreamServer.getFPS(), 
                "7.4", 
                "false");

            //Serial.println("JSON response");
            //Serial.println(json);

            memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
            ws_pkt.payload = (uint8_t *)json;
            ws_pkt.len = strlen(json);
            ws_pkt.type = HTTPD_WS_TYPE_TEXT;

            res = httpd_ws_send_frame(req, &ws_pkt);

            if (res != ESP_OK) {
                Serial.printf("httpd_ws_send_frame failed with %d", res);
                Serial.println();
                return res;
            }            
        }

        if (strncmp((const char*)buf,"data", 4) == 0 ) {

            gamepad_raw_data d;
            memcpy(&d, &buf[5], sizeof(d));

            ::camerabrick::Gamepad.updateData(d);

            //Serial.printf("%5.2f %5.2f %5.2f %5.2f %5.2f %5.2f", d.LX, d.LY, d.RX, d.RY, d.LT, d.RT);
            //Serial.println();
        }

        return res;     
    }

    esp_err_t WebServer::file_handler(httpd_req_t *req) {

        for (int i=0; i<app_camera_files_count; i++) {

            if (strcmp(req->uri, app_camera_files[i].file_name) == 0) {

                httpd_resp_set_type(req, app_camera_files[i].content_type);

                if (app_camera_files[i].content_encoding != nullptr)
                    httpd_resp_set_hdr(req, "Content-Encoding", app_camera_files[i].content_encoding);

                httpd_resp_send_chunk(req, (const char*)app_camera_files[i].content, app_camera_files[i].content_size);
                httpd_resp_send_chunk(req, nullptr, 0);

                return ESP_OK;
            }
        }            

        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    
    esp_err_t WebServer::root_config_handler(httpd_req_t *req) {

        httpd_resp_set_type(req, "application/json");

        JsonDocument json;

        std::string hostname = ::camerabrick::WiFiManager.getHostname() + ".local";

        json["stream_url"] = std::string("http://") + hostname + std::string(":8080/stream");
        json["settings_url"] = std::string("http://") + hostname + std::string("/settings");
        json["websocket_url"] = std::string("ws://") + hostname + std::string("/ws");
        json["gamepad_enabled"] = ::CameraBrick.getProfile()->isGamepadEnabled();
        json["fullscreen_enabled"] = true;
        json["rotation"] = ::camerabrick::ESP32Camera.getConfigRotation(); 

        JsonArray jsonComponents = json["components"].to<JsonArray>();
        
        auto callback = [&jsonComponents](ConfigComponent* comp) {
            JsonObject jsonComp = jsonComponents.add<JsonObject>();
            jsonComp["name"] = comp->getName();
            jsonComp["enabled"] = comp->isEnabled();
        };        

        ::CameraBrick.iterateComponents(callback);

        return httpd_resp_send_json_chunk(req, json);
    }

    esp_err_t WebServer::component_config_handler_get(httpd_req_t *req) {

        httpd_resp_set_type(req, "application/json");

        // extract component name from '/config/<name>' uri
        std::string name = std::string(req->uri).substr(8); 

        ConfigComponent *component = ::CameraBrick.getConfigComponent(name);

        if (component != nullptr) {

            JsonDocument json = component->saveSettingsToJson();

            return httpd_resp_send_json_chunk(req, json);
        }

        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    esp_err_t WebServer::component_config_handler_post(httpd_req_t *req) {

        // extract component name from '/config/<name>' uri
        std::string name = std::string(req->uri).substr(8); 

        ConfigComponent *component = ::CameraBrick.getConfigComponent(name);

        // read JSON from request content
        size_t total_len = req->content_len;
        char *buffer = new char[total_len+1];

        esp_err_t res = httpd_req_recv(req, buffer, total_len);
        buffer[total_len] = '\0';

        if (res > 0) {

            JsonDocument json;
            DeserializationError error = deserializeJson(json, buffer);

            if ( (!error) && (component != nullptr) ) {

                if (component->loadSettingsFromJson(json)) {

                    component->saveSettingsToStorage();
                    component->applySettings();

                    // return actual updated JSON config 
                    res = component_config_handler_get(req);
                }
            }
        }

        delete [] buffer;
        
        if (res != ESP_OK)
            httpd_resp_send_500(req);

        return res;
    }

}