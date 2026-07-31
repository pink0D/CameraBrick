//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _STREAM_SERVER_H
#define _STREAM_SERVER_H

#include <Arduino.h>

#include "ESP32Camera.h"

#include <esp_http_server.h>
#include <freertos/semphr.h>

namespace camerabrick::comp {

    class StreamServer {

        public:
            StreamServer() {};

            bool begin();

            int getFPS() {
                return fps;
            };


        private:
            
            httpd_handle_t stream_httpd = nullptr;

            esp_err_t handler(httpd_req_t *req);

            int fps = 0;

    };
}

namespace camerabrick {
    extern comp::StreamServer StreamServer;  
}

#endif