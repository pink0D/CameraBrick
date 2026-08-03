//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//


#include "ESP32Camera.h"
#include "ESP32CameraConfig.h"
#include "CameraBrick.h"

#include <img_converters.h>

//global instance
namespace camerabrick {
    camerabrick::comp::ESP32Camera ESP32Camera;  

    static class : public EnumMapper<ESP32CameraType> {
            void init() override {
                addMap(ESP32CameraType::ESP32CAM_OV2640, "ESP32CAM OV2640");
                addMap(ESP32CameraType::M5STACK_ATOM_S3R_M12_OV3660, "M5Stack AtomS3R-M12 OV3660");
                addMap(ESP32CameraType::M5STACK_ATOM_S3R_GC0308, "M5Stack AtomS3R-CAM GC0308");
                addMap(ESP32CameraType::M5STACK_TIMER_CAMERA_OV3660, "M5Stack Timer Camera OV3660");

                setDefaultEnum(ESP32CameraType::ESP32CAM_OV2640);
                setDefaultString("");
            };
    } ESP32CameraTypeMapper;
    
    static class : public EnumMapper<framesize_t> {
            void init() override {
                addMap(FRAMESIZE_QQVGA, "QQVGA 160x120");
                addMap(FRAMESIZE_QVGA, "QVGA 320x240");
                addMap(FRAMESIZE_CIF, "CIF 400x296");
                addMap(FRAMESIZE_HVGA, "HVGA 480x320");
                addMap(FRAMESIZE_VGA, "VGA 640x480");
                addMap(FRAMESIZE_SVGA, "SVGA 800x600");
                addMap(FRAMESIZE_XGA, "XGA 1024x768");
                addMap(FRAMESIZE_HD, "HD 1280x720");
                addMap(FRAMESIZE_SXGA, "SXGA 1280x1024");
                addMap(FRAMESIZE_UXGA, "UXGA 1600x1200");
                addMap(FRAMESIZE_FHD, "FHD 1920x1080");

                setDefaultEnum(FRAMESIZE_QVGA);
                setDefaultString("");
            };
    } ESP32ResolutionMapper;
}

namespace camerabrick::comp {

    void ESP32Camera::begin() {        

        switch (::CameraBrick.getProfile()->cameraType) {
            
            case ESP32CameraType::ESP32CAM_OV2640:
                config = camerabrick::config::ESP32CAM_OV2640();
                break;

            case ESP32CameraType::M5STACK_ATOM_S3R_M12_OV3660:
                config = camerabrick::config::M5STACK_ATOM_S3R_M12_OV3660();
                break;

            case ESP32CameraType::M5STACK_ATOM_S3R_GC0308:
                config = camerabrick::config::M5STACK_ATOM_S3R_GC0308();
                break;

            case ESP32CameraType::M5STACK_TIMER_CAMERA_OV3660:
                config = camerabrick::config::M5STACK_TIMER_CAMERA_OV3660();
                break;
        }

        if (config.ledPin > 0) {
            pinMode(config.ledPin, OUTPUT);
            digitalWrite(config.ledPin, config.ledPinValueOff);
        }

        if (config.powerPin > 0) {
            pinMode(config.powerPin, OUTPUT);
            digitalWrite(config.powerPin, config.powerPinValueOff);
        }
    
        ConfigComponent::begin();
    }

    void ESP32Camera::blink(int count) {

        if (config.ledPin < 0)
            return;

        for (int i=0; i<count; i++) {
            digitalWrite(config.ledPin, config.ledPinValueOn);
            vTaskDelay(pdMS_TO_TICKS(500));
            digitalWrite(config.ledPin, config.ledPinValueOff);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
        
    bool ESP32Camera::startCapture()
    {

        if (config.powerPin > 0) {
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

    void ESP32Camera::stopCapture() {

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
        
        //Frame *frame = new Frame();
        
        this->frame.timestamp = fb->timestamp;
        this->frame.jpegBuffer = jpg_buf;
        this->frame.jpegBufferLength = jpg_buf_len;

        return &frame;
    }

    void ESP32Camera::releaseFrame(ESP32Camera::Frame *frame)
    {
        //if (frame != nullptr)
        //    delete frame;

        if (fb) {
            esp_camera_fb_return(fb);
            fb = nullptr;
            jpg_buf = nullptr;
        } else if (jpg_buf) {
            free(jpg_buf);
            jpg_buf = nullptr;
        }
    }

    bool ESP32Camera::loadSettingsFromJson(JsonDocument &json) {

        config.espCameraConfig.frame_size = ESP32ResolutionMapper.mapStringToEnum(json["resolution"]);
        config.mirrorX = json["mirrorX"];
        config.mirrorY = json["mirrorY"];
        config.rotation = max(0, min(359,  (int)(json["rotation"])));

        return true;
    }

    JsonDocument ESP32Camera::saveSettingsToJson() {
        
        JsonDocument json;

        json["cameraType"] = ESP32CameraTypeMapper.mapEnumToString(::CameraBrick.getProfile()->cameraType);
        json["resolution"] = ESP32ResolutionMapper.mapEnumToString(config.espCameraConfig.frame_size);
        json["mirrorX"] = config.mirrorX;
        json["mirrorY"] = config.mirrorY;
        json["rotation"] = config.rotation;

        return json;
    }

}