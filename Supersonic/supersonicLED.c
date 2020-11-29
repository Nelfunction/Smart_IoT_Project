#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
  int trig = 8 ;
  int echo = 9 ;

      int ledpin = 1;

  int start_time, end_time ;
  float distance ;

  if (wiringPiSetup() == -1) exit(1) ;

  pinMode(trig, OUTPUT) ;
  pinMode(echo , INPUT) ;

  pinMode(ledpin, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(19);
    pwmSetRange(1000000);

  while(1) {
    digitalWrite(trig, LOW) ;
    delay(500) ;
    digitalWrite(trig, HIGH) ;
    delayMicroseconds(10) ;
    digitalWrite(trig, LOW) ;
    while (digitalRead(echo) == 0) ;
    start_time = micros() ;
    while (digitalRead(echo) == 1) ;
    end_time = micros() ;
    distance = (end_time - start_time) / 29. / 2. ;
    printf("distance %.2f cm\n", distance) ;

    pwmWrite(ledpin,1000000 - (100000/(int)distance));

  }
  return 0 ;
}