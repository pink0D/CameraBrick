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
#include <string>
#include <esp_http_server.h>

namespace camerabrick {

    class StreamServer {

        public:
            StreamServer() {};

            bool begin();

            int getFPS() {
                return fps;
            };

            bool isActive() {
                return streamActive;
            };

            void resetStreamTimeout();
            void stopStream();
            void updateToken(std::string token);
            bool isValidToken(std::string token);

        private:
            
            httpd_handle_t stream_httpd = nullptr;

            esp_err_t handler(httpd_req_t *req);

            int fps = 0;

            bool streamActive = false;

            int64_t streamTimeout = 0;

            std::string token;

    };
}

namespace camerabrick::comp {
    extern camerabrick::StreamServer StreamServer;  
}

#endif