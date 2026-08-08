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

#include "CameraBrick.h"
#include "ESP32Camera.h"

#define PART_BOUNDARY "CameraFrameBoundary"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

//global instance
namespace camerabrick::comp {
    camerabrick::StreamServer StreamServer;  
}

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
        config.ctrl_port = 38080;

        Serial.print("Starting stream server on port ");
        Serial.println(config.server_port);

        if (httpd_start(&stream_httpd, &config) == ESP_OK) {
            httpd_register_uri_handler(stream_httpd, &stream_uri);
        }

        Serial.println("Stream server started");       

        return true;
    }

    esp_err_t StreamServer::handler(httpd_req_t *req) {

        // only one active stream due to performance limitations of ESP32 Camera
        if (streamActive) {
            return ESP_FAIL;
        }

        bool enableCapture = true;
        std::string req_token = "";

        char*  buf;
        size_t buf_len;
        char param_value[64];

        buf_len = httpd_req_get_url_query_len(req) + 1;
        buf = (char*) malloc(buf_len);

        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {

            if (httpd_query_key_value(buf, "capture", param_value, sizeof(param_value)) == ESP_OK) {
                if (strcmp(param_value,"false") == 0) {
                    enableCapture = false;
                    Serial.println("Camera capture disabled with query param: capture=false");
                }
            }

            if (httpd_query_key_value(buf, "token", param_value, sizeof(param_value)) == ESP_OK) {
                req_token = param_value;
            }
        }

        free(buf);


        Serial.print("Camera stream started, token=");
        Serial.println(req_token.c_str());
        this->streamActive = true;

        ::CameraBrick.getProfile()->start();

        esp_err_t res = ESP_OK;
        char *part_buf[128];

        res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
        if (res != ESP_OK) {
            this->streamActive = false;
            return res;
        }

        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        httpd_resp_set_hdr(req, "Cache-Control", "no-store");
        httpd_resp_set_hdr(req, "X-Framerate", "60");

        if (!::camerabrick::comp::ESP32Camera.startCapture()) {
            Serial.println("Camera init failed");
            this->streamActive = false;
            return ESP_FAIL;
        }

        this->fps = 0;
        this->resetStreamTimeout();

        int fps_count_time = 1; // refresh fps every 1 second
        int frame_count = 0;
        int64_t time_reset_frame_count = esp_timer_get_time() + fps_count_time * 1000000;

        while (true) {

            // check token so only the latest token can be used to get streaming data
            // this prevents camera capture being blocked by browser's background connections left open for closed tabs

            if (!isValidToken(req_token)) {
                Serial.print("Camera stream token=");
                Serial.print(req_token.c_str());
                Serial.println(" is invalid, stopping stream");
                break;
            }

            if (enableCapture) {

                /*
                auto dma_free = heap_caps_get_free_size(MALLOC_CAP_DMA);
                auto dma_largest = heap_caps_get_largest_free_block(MALLOC_CAP_DMA);

                // delay when DMA pressure detected
                if (dma_free < 12 * 1024 || dma_largest < 4 * 1024) {
                    vTaskDelay(pdMS_TO_TICKS(250));
                }
                */

                ESP32Camera::Frame *frame = ::camerabrick::comp::ESP32Camera.captureFrame();

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

                ::camerabrick::comp::ESP32Camera.releaseFrame(frame);
                
                if (res == ESP_OK) {
                    frame_count++;
                }
                else{
                    // delay if capture was not successful
                    vTaskDelay(pdMS_TO_TICKS(100));
                    res = ESP_OK;
                }

            } else {
                // capture disabled
                frame_count++;
                vTaskDelay(pdMS_TO_TICKS(40));
            }

            ::CameraBrick.cameraSync(); // call processing inside CameraBrick after the frame has been sent over WiFi           

            int64_t time_now = esp_timer_get_time();

            // stop streaming if timeout was not updated by ping in websocket handler
            if (time_now > this->streamTimeout) {
                break;
            }


            if (time_now > time_reset_frame_count) {
                time_reset_frame_count = time_now + fps_count_time*1000000;
                this->fps = int( ((float)frame_count) / ((float)fps_count_time) );
                frame_count = 0;

                Serial.print("Stream FPS=");
                Serial.println(this->fps);
            }
            
            if (res != ESP_OK) {
                break;
            }
        }

        ::camerabrick::comp::ESP32Camera.stopCapture();
        ::CameraBrick.getProfile()->stop();

        this->fps = 0;
        this->streamActive = false;

        Serial.print("Camera stream stopped, token=");
        Serial.println(req_token.c_str());

        return res;
    }

    void StreamServer::resetStreamTimeout() {
        streamTimeout = esp_timer_get_time() + 2000000;
    };

    void StreamServer::stopStream() {
        streamTimeout = 0;
    }

    void StreamServer::updateToken(std::string token) {
        this->token = token;
    }

    bool StreamServer::isValidToken(std::string token) {
        return ((token == "secret") || (token == this->token));
    }


}