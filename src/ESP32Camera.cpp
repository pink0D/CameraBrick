//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//


#include "ESP32Camera.h"

#include "img_converters.h"
#include "fb_gfx.h"


namespace camerabrick {
        
    bool ESP32Camera::begin()
    {

        if (config.ledPin > 0) {
            pinMode(config.ledPin, OUTPUT);
            digitalWrite(config.ledPin, config.ledPinValueOff);
        }

        if (config.powerPin > 0) {
            pinMode(config.powerPin, OUTPUT);
            digitalWrite(config.powerPin, config.powerPinValueOn);
            vTaskDelay(pdMS_TO_TICKS(config.powerDelayMillis));
        }

        camera_config_t camera_config = config.espCameraConfig;

        esp_err_t err = esp_camera_init(&camera_config);
        if (err != ESP_OK) {
            Serial.printf("Camera init failed with error 0x%x", err);
            Serial.println();
            return false;
        }

        sensor_t *s = esp_camera_sensor_get();

        if (config.espCameraConfig.frame_size == FRAMESIZE_CIF) {
            // set sensor resolution to match capture size and aspect ratio for CIF resolution

            err = s->set_res_raw(s, 2, 0, 0, 0, 0, 0, 400, 296, 400, 296, false, false);
            if (err != ESP_OK) {
                Serial.printf("Could not set camera window with error 0x%x", err);
                return false;
            }

            err = s->set_framesize(s, (framesize_t) FRAMESIZE_CIF);
            if (err != ESP_OK) {
                Serial.printf("Could not set frame size with error 0x%x", err);
                return false;
            }
        }     

        if (config.mirrorX)
            s->set_hmirror(s, 1);

        if (config.mirrorY)
            s->set_vflip(s, 1);
      
        return true;
    }

    void ESP32Camera::stop() {

        esp_camera_deinit();

        if (config.powerPin > 0) {
            digitalWrite(config.powerPin, config.powerPinValueOff);
        }

        Serial.println("Camera capture stopped");
    }

    ESP32Camera::Frame* ESP32Camera::captureFrame()
    {
        fb = esp_camera_fb_get();
        jpg_buf_len = 0;
        jpg_buf = nullptr;

        if (!fb) {
            Serial.println("Camera capture failed");
            return nullptr;
        } 

        if (fb->format != PIXFORMAT_JPEG) {
            bool jpeg_converted = frame2jpg(fb, 80, &jpg_buf, &jpg_buf_len);
            esp_camera_fb_return(fb);
            fb = nullptr;
            if (!jpeg_converted) {
                Serial.println("JPEG compression failed");
                releaseFrame(nullptr);
                return nullptr;
            }
        } else {
            jpg_buf_len = fb->len;
            jpg_buf = fb->buf;
        }
        
        Frame *frame = new Frame();
        
        frame->timestamp = fb->timestamp;
        frame->jpegBuffer = jpg_buf;
        frame->jpegBufferLength = jpg_buf_len;

        return frame;
    }

    void ESP32Camera::releaseFrame(ESP32Camera::Frame *frame)
    {
        if (frame != nullptr)
            delete frame;

        if (fb) {
            esp_camera_fb_return(fb);
            fb = nullptr;
            jpg_buf = nullptr;
        } else if (jpg_buf) {
            free(jpg_buf);
            jpg_buf = nullptr;
        }
    }

}