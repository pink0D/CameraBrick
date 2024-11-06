/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ENABLE_CAMERA   // comment this define if you do not connect camera module or you do no need web streaming 
                        // (disabling this greatly reduces power consuption and module heating)

#define ENABLE_SERVO    // comment this define to disable servo control 
                        // (disabling this slightly increases streaming FPS)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(ENABLE_CAMERA)

#include <WiFi.h>
#include <ESPmDNS.h>
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
#include "esp_camera_server.h"
#include "secrets.h"

// WiFi credentials are set in secrets.h
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *hostname = WIFI_HOSTNAME;

#endif

#if defined(ENABLE_SERVO)

#define PIN_SERVO 12 // GeekServo signal
#define PIN_C1    14 // C1 from HUB
#define PIN_C2    15 // C2 from HUB

#define ANGLE_MAX         90 // maximum servo rotation - used to limit steering for CV joints 
#define ANGLE_DELTA_MIN   3  // minimum change in angle to move servo - used to avoid fluttering
#define PWM_DEADZONE      20 // MK controller has significant deadzone

#include <ESP32Servo.h>
#include "pwm_reader.h"

Servo servo;
pwm_state_t pwm[2];
int servo_angle = 90;

double normalize_input(double c) {
  return max(0.0, c - (((double)PWM_DEADZONE) / 100.0 ) ) / (1.0 - (((double)PWM_DEADZONE) / 100.0 ));
}

void servo_update(void *p) {
  while (1) {

    double new_c1 = normalize_input(get_duty(&pwm[0]));
    double new_c2 = normalize_input(get_duty(&pwm[1]));
    int new_angle = 90.0 + ((double)ANGLE_MAX) * (-new_c1 + new_c2);

    if (new_angle < 0) new_angle = 0;
    if (new_angle > 180) new_angle = 180;

    if (abs(new_angle - servo_angle) >= ANGLE_DELTA_MIN)
      servo_angle = new_angle;

    servo.write(servo_angle);

    //Serial.println("c1 = " + String(new_c1) + ", c2 = " + String(new_c2));

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
#endif

void setup() {

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

#if defined(ENABLE_CAMERA)

#ifdef DISABLE_ESP32_BROWNOUT_DETECTOR
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
#endif


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
#endif

#if defined(ENABLE_SERVO)
  servo.setPeriodHertz(50);    
  servo.attach(PIN_SERVO, 1000, 2000);

  setup_pwm_reader(&pwm[0], PIN_C1);
  setup_pwm_reader(&pwm[1], PIN_C2);

  xTaskCreatePinnedToCore(servo_update,"servo_update",10*1024,NULL,2,NULL,1);
#endif

#if defined(ENABLE_CAMERA)

  if (setupCameraServer()) {
    startCameraServer();
    Serial.print("Camera Ready! Use 'http://");
    Serial.print(hostname);
    Serial.println(".local' to connect");
  }

#endif

}

void loop() {

#if defined(ENABLE_CAMERA)
  int fps = getStreamingFPS();
  if (fps > 0)
    Serial.println("FPS = " + String(fps));
#endif

  delay(1000);
}
