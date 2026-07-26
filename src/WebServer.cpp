//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "WebServer.h"
#include "StreamServer.h"

#include <esp_http_server.h>

namespace camerabrick {

    struct __attribute__((packed)) gamepad_data {
        double timestamp;
        float LX;
        float LY;
        float RX;
        float RY;
        float LT;
        float RT;
        uint16_t buttons;      
    };

    bool WebServer::begin() {

        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.max_uri_handlers = 16;

        httpd_uri_t stream_uri = {
            .uri = "/ws",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->websocket_handler(req);
            },
            .user_ctx = this,
            .is_websocket = true
        };  

        config.server_port = 80;
        config.ctrl_port = 30080;

        Serial.print("Starting web server on port ");
        Serial.println(config.server_port);

        if (httpd_start(&web_httpd, &config) == ESP_OK) {
            httpd_register_uri_handler(web_httpd, &stream_uri);
        }

        Serial.println("Web server started");

        return true;
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

            int rssi = -40;
            
            sprintf(json, json_fmt, 
                rssi, 
                &buf[5], // timestamp sent thru ws to calculate ping in browser
                StreamServer::instance().getFPS(), 
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
            gamepad_data d;
            memcpy(&d, &buf[5], sizeof(d));

            Serial.printf("%5.2f %5.2f %5.2f %5.2f %5.2f %5.2f", d.LX, d.LY, d.RX, d.RY, d.LT, d.RT);
            Serial.println();
        }

        return res;     
    }
    
}