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
#include <ArduinoJson.h>

namespace camerabrick {

    class WebServer {

        public:
            WebServer() {};

            bool begin();

        private:            

            httpd_handle_t web_httpd = nullptr;

            esp_err_t websocket_handler(httpd_req_t *req);
            esp_err_t file_handler(httpd_req_t *req);
            esp_err_t root_config_handler(httpd_req_t *req);
            esp_err_t component_config_handler_get(httpd_req_t *req);
            esp_err_t component_config_handler_post(httpd_req_t *req);

            esp_err_t httpd_resp_send_json_chunk(httpd_req_t *req, JsonDocument &json);

    };
}

namespace camerabrick::comp {
    extern camerabrick::WebServer WebServer;  
}


#endif