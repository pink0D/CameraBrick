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

#include "web/app_camera_files.h"

//global instance
namespace camerabrick::comp {
    camerabrick::WebServer WebServer;  
}

namespace camerabrick {    

    bool WebServer::begin() {

        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.max_uri_handlers = 16;
        config.uri_match_fn = httpd_uri_match_wildcard;

        httpd_uri_t ws_uri = {
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

        httpd_uri_t component_config_options_uri = {
            .uri = "/config/*",
            .method = HTTP_OPTIONS,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->component_config_handler_options(req);
            },
            .user_ctx = this,
        };  

        httpd_uri_t token_uri = {
            .uri = "/token",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->token_get_handler(req);
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
            httpd_register_uri_handler(web_httpd, &ws_uri);
            httpd_register_uri_handler(web_httpd, &root_config_uri);
            httpd_register_uri_handler(web_httpd, &component_config_get_uri);
            httpd_register_uri_handler(web_httpd, &component_config_post_uri);
            httpd_register_uri_handler(web_httpd, &component_config_options_uri);
            httpd_register_uri_handler(web_httpd, &token_uri);
            httpd_register_uri_handler(web_httpd, &files_uri);
        }
        
        addWebFiles(app_camera_web_data);

        Serial.println("Web server started");

        return true;
    }

    esp_err_t WebServer::httpd_resp_send_json_chunk(httpd_req_t *req, JsonDocument &json) {

        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        httpd_resp_set_hdr(req, "Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");

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

        if (ws_pkt.len < 4 + 1 + 8) {
            Serial.println("WS Incoming packet invalid");
            return ESP_FAIL;
        }

        char cmd[5];
        strncpy(cmd, (const char*) buf, 4);
        cmd[4] = '\0';

        char ws_token[9];
        strncpy(ws_token, (const char*) &buf[5], 8);
        ws_token[8] = '\0';

        // stop command is executed before token check
        if (strncmp(cmd, "stop", 4) == 0) {
            ::camerabrick::comp::StreamServer.stopStream();
            this->token = ""; // forces websocket to close
        }

        // check valid token or close websocket
        if (this->token != ws_token) {

            Serial.print("Websocket closing: token=");
            Serial.println(ws_token);

            int sockfd = httpd_req_to_sockfd(req);
            if (sockfd != -1) {
                httpd_sess_trigger_close(req->handle, sockfd);
            }
            return ESP_FAIL;
        }

        if (strncmp(cmd, "ping", 4) == 0) {

            ::camerabrick::comp::StreamServer.resetStreamTimeout(); // update stream timeout
            
            const char* json_fmt = "{\"rssi\":%d, \"ping\":%s, \"fps\":%d, \"voltage\":%.1f, \"low_voltage_flag\":%s}";
            char json[256];

            int rssi = ::camerabrick::comp::WiFiManager.getRSSI();
            
            sprintf(json, json_fmt, 
                rssi, 
                &buf[4+1+8+1], // timestamp sent thru ws to calculate ping in browser
                ::camerabrick::comp::StreamServer.getFPS(), 
                ::CameraBrick.getProfile()->getVoltage(), 
                ::CameraBrick.getProfile()->isLowVoltage() ? "true" : "false");

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

        if (strncmp(cmd, "data", 4) == 0 ) {

            if (ws_pkt.len < 4 + 1 + 8 + 1 + sizeof(gamepad_raw_data)) {
                Serial.println("WS Incoming packet invalid (wrong data length)");
                return ESP_FAIL;
            }

            gamepad_raw_data d;
            memcpy(&d, &buf[4+1+8+1], sizeof(d));

            ::camerabrick::comp::Gamepad.updateData(d);

            //Serial.printf("%5.2f %5.2f %5.2f %5.2f %5.2f %5.2f", d.LX, d.LY, d.RX, d.RY, d.LT, d.RT);
            //Serial.println();
        }

        return res;     
    }

    void WebServer::addWebFiles(const web_data &files) {

        webData.push_back(files);
    }

    esp_err_t WebServer::file_handler(httpd_req_t *req) {

        for (const auto& web : webData) {
            
            for (int i=0; i<web.count; i++) {

                if (strcmp(req->uri, web.files[i].file_name) == 0) {

                    httpd_resp_set_type(req, web.files[i].content_type);

                    if (web.files[i].content_encoding != nullptr)
                        httpd_resp_set_hdr(req, "Content-Encoding", web.files[i].content_encoding);

                    httpd_resp_send_chunk(req, (const char*)web.files[i].content, web.files[i].content_size);
                    httpd_resp_send_chunk(req, nullptr, 0);

                    return ESP_OK;
                }
            }         
        }

        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    esp_err_t WebServer::token_get_handler(httpd_req_t *req) {

        JsonDocument json;

        char token[16];
        sprintf(token,"%08X", esp_random());

        json["token"] = token;

        updateToken(token);

        return httpd_resp_send_json_chunk(req, json);
    }

    void WebServer::updateToken(std::string token) {
        this->token = token;
    }

    bool WebServer::isValidToken(std::string token) {
        return ((token == "_secret_") || (token == this->token));
    }

    
    esp_err_t WebServer::root_config_handler(httpd_req_t *req) {

        JsonDocument json;

        std::string hostname = ::camerabrick::comp::WiFiManager.getHostname() + ".local";

        json["token_url"] = std::string("http://") + hostname + std::string("/token");
        json["stream_url"] = std::string("http://") + hostname + std::string(":8080/stream");
        json["settings_url"] = std::string("http://") + hostname + std::string("/settings");
        json["websocket_url"] = std::string("ws://") + hostname + std::string("/ws");
        json["gamepad_enabled"] = ::CameraBrick.getProfile()->isGamepadEnabled();
        json["fullscreen_enabled"] = true;
        json["rotation"] = ::camerabrick::comp::ESP32Camera.getRotation(); 

        JsonArray jsonComponents = json["components"].to<JsonArray>();
        
        auto callback = [&jsonComponents](ConfigComponent* comp) {
            JsonObject jsonComp = jsonComponents.add<JsonObject>();
            jsonComp["name"] = comp->getName();
            jsonComp["enabled"] = comp->isEnabled();
        };        

        ::CameraBrick.iterateComponents(callback);

        return httpd_resp_send_json_chunk(req, json);
    }

    esp_err_t WebServer::component_config_handler_options(httpd_req_t *req) {

        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_send(req, NULL, 0);

        return ESP_OK;
    }

    esp_err_t WebServer::component_config_handler_get(httpd_req_t *req) {

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