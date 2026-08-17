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

#include <list>

#ifndef FILE_DATA_STRUCT_DEFINED
#define FILE_DATA_STRUCT_DEFINED

struct web_file_data {
    const char*     file_name;
    size_t          content_size;
    const char*     content_type;
    const char*     content_encoding;
    const uint8_t*  content;
};

struct web_data {
    const web_file_data* files;
    size_t               count;
};

#endif 

namespace camerabrick {

    class WebServer {

        public:
            WebServer() {};

            bool begin();

            void addWebFiles(const web_data &files);

            void updateToken(std::string token);
            bool isValidToken(std::string token);

        private:            

            httpd_handle_t web_httpd = nullptr;

            esp_err_t websocket_handler(httpd_req_t *req);

            esp_err_t file_handler(httpd_req_t *req);

            esp_err_t root_config_handler(httpd_req_t *req);

            esp_err_t component_config_handler_options(httpd_req_t *req);
            esp_err_t component_config_handler_get(httpd_req_t *req);
            esp_err_t component_config_handler_post(httpd_req_t *req);

            esp_err_t token_get_handler(httpd_req_t *req);

            esp_err_t httpd_resp_send_json_chunk(httpd_req_t *req, JsonDocument &json);

            std::list<web_data> webData;

            std::string token;

    };
}

namespace camerabrick::comp {
    extern camerabrick::WebServer WebServer;  
}


#endif