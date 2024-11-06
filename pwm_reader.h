#ifndef _pwm_reader_h
#define _pwm_reader_h

#define AVG_ARRAY_SIZE 10             // used to calculate duty cycle as average from N last pulses
#define MAX_CHANGE_TIME_MILLIS 10     // used as pulse timeout for duty cycle calculations for 0 and 100% (continiuous LOW or HIGH)

#define PWM_READERS_MAX 10

struct pwm_state_t {
  int pin;
  int state;
  unsigned long lastChangeTime;

  unsigned long timeChangeHigh;
  unsigned long timeChangeLow;

  int index;
  unsigned long timeHigh[AVG_ARRAY_SIZE];
  unsigned long timeCycle[AVG_ARRAY_SIZE];

  double duty;
};

void setup_pwm_reader(pwm_state_t *s, int pin);
double get_duty(pwm_state_t *s);

#endif