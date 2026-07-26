//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _WEB_SERVER_H
#define _WEB_SERVER_H

#include <Arduino.h>

#include <esp_http_server.h>

namespace camerabrick {

    class WebServer {

        public:
            static WebServer& instance() {
                static WebServer obj;
                return obj;
            }

            bool begin();

        private:
            WebServer() {};

            httpd_handle_t web_httpd = nullptr;

            esp_err_t websocket_handler(httpd_req_t *req);

    };
}


#endif