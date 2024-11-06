#include <Arduino.h>

#include "pwm_reader.h"

bool duty_task_created = false;
pwm_state_t* states[PWM_READERS_MAX];
int num_readers = 0;

void duty_reset(void *param) {
  while (1) {

    unsigned long time = micros();

    for (int i=0; i<num_readers; i++) {
      pwm_state_t* s = states[i];
      if (time > s->lastChangeTime + MAX_CHANGE_TIME_MILLIS * 1000) { 
        // reset duty to last known pin state after timeout if no pwm signal is present
        s->duty = s->state;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(MAX_CHANGE_TIME_MILLIS));
  }
}

void init_pwm_state(pwm_state_t *s, int pin){
  s->pin = pin;
  s->state = 0;
  s->index = 0;
  s->duty = 0;
  s->lastChangeTime = 0;
}

// excludes zeros and out of cycle values (>2500 micros) from average calculation
inline unsigned long smart_avg(unsigned long *a){

  unsigned long sum = 0;
  int count = 0;

  for (int i=0; i<AVG_ARRAY_SIZE; i++){
    unsigned long val = a[i];

    if (0 < val && val < 2500) {
      sum += val;
      count += 1;
    }
  }

  if (count == 0)
     return 0;

  return sum / count;
}

// calculate duty cycle based on saved timings
inline double calculate_duty(pwm_state_t *s) {
  double duty = 0;

  double avg_cycle = smart_avg(s->timeCycle);
  double avg_high  = smart_avg(s->timeHigh);
  if (avg_cycle > 0) {
    duty = avg_high / avg_cycle;
  } else
  {
    duty = 0;
  }

  return duty;
}

// save pulse timings for duty cycle calculation
inline void pin_update_pwm(pwm_state_t *s) {

  int value = digitalRead(s->pin);

  if (value == s->state) return; //no change!

  unsigned long time = micros();
  int i = s->index;
  
  if (value == HIGH) {
    //save cycle time = time since previous signal change to HIGH
    s->timeCycle[i] = time - s->timeChangeHigh;
    s->timeChangeHigh = time;
  } 

  if (value == LOW) {
    // save duty cycle = time since signal changed to HIGH
    s->timeHigh[i] = time - s->timeChangeHigh;
    s->timeChangeLow = time;

    s->index += 1;
    if (s->index == AVG_ARRAY_SIZE) {
      s->index = 0;
    }
  }

  s->state = value;
  s->duty = calculate_duty(s);
  s->lastChangeTime = time;
}

void interrupt_pwm(void *p) {
  pin_update_pwm((pwm_state_t*)p);
}

void setup_pwm_reader(pwm_state_t *s, int pin) {
  if (num_readers == PWM_READERS_MAX) {
    Serial.println("No more PWM readers available");
    return;
  }

  init_pwm_state(s,pin);
  pinMode(pin, INPUT_PULLDOWN);
  attachInterruptArg(digitalPinToInterrupt(pin), interrupt_pwm, s, CHANGE);

  states[num_readers++] = s;

  if (!duty_task_created) {
    xTaskCreatePinnedToCore(duty_reset,"duty_reset",10*1024,NULL,2,NULL,1);
    duty_task_created = true;
  }
}

double get_duty(pwm_state_t *s) {
  return s->duty;
}
