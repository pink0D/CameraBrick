//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "StreamServer.h"

#include <esp_http_server.h>

#define PART_BOUNDARY "CameraFrameBoundary"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

namespace camerabrick {

    bool StreamServer::begin() {

        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.max_uri_handlers = 4;

        httpd_uri_t stream_uri = {
            .uri = "/stream",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<StreamServer*>(req->user_ctx)->handler(req);
            },
            .user_ctx = this
        };  

        config.server_port = 8080;
        config.ctrl_port = 32000;

        Serial.print("Starting stream server on port ");
        Serial.println(config.server_port);

        if (httpd_start(&stream_httpd, &config) == ESP_OK) {
            httpd_register_uri_handler(stream_httpd, &stream_uri);
        }

        Serial.println("Stream server started");

        return true;
    }

    esp_err_t StreamServer::handler(httpd_req_t *req) {

        esp_err_t res = ESP_OK;
        char *part_buf[128];

        res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
        if (res != ESP_OK) {
            return res;
        }

        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        httpd_resp_set_hdr(req, "X-Framerate", "60");

        if (!ESP32Camera::instance().begin()) {
            Serial.println("Camera init failed");
            return ESP_FAIL;
        }

        while (true) {

            ESP32Camera::Frame *frame = ESP32Camera::instance().captureFrame();

            if (frame == nullptr) {
                Serial.println("Camera capture failed");
                res = ESP_FAIL;
            }
                
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
            }

            if (res == ESP_OK) {

                size_t hlen = snprintf((char *)part_buf, 128, 
                    _STREAM_PART, 
                    frame->jpegBufferLength, 
                    frame->timestamp.tv_sec, 
                    frame->timestamp.tv_usec);

                res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
            }

            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, (const char *)frame->jpegBuffer, frame->jpegBufferLength);
            }

            ESP32Camera::instance().releaseFrame(frame);
            
            if (res != ESP_OK) {
                break;
            }
        }

        ESP32Camera::instance().stop();

        return res;
    }
    
}