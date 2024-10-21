#include <WiFi.h>
#include <ESPmDNS.h>
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems

#include "esp_camera_server.h"
#include "secrets.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *hostname = "fpvbrick";

void setup() {
#ifdef DISABLE_ESP32_BROWNOUT_DETECTOR
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
#endif

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    MDNS.addService("http", "tcp", 80);
  }
  Serial.println("mDNS responder started");

  if (setupCameraServer()) {
    startCameraServer();
    Serial.print("Camera Ready! Use 'http://");
    Serial.print(hostname);
    Serial.println(".local' to connect");
  }
}

void loop() {
  int fps = getStreamingFPS();
  if (fps > 0)
    Serial.println("FPS = " + String(fps));

  delay(1000);
}
