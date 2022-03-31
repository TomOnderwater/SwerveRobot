#ifndef fullServo_h
#define fullServo_h

#include "Arduino.h"
#include <Servo.h>


class fullServo
{
  public:
    fullServo(byte s_pin);
    void setAccuracy(int _treshold);
    void write(int val);
    int move(long degree);
    long getAngle();
    int getTurns();
    int getAngleSpeed();
    int upgradeAngle(int a);
    int radToDeg(float a);
    float angleSpeed;
    void updateAngle(float t, float v);
    int getLiveAngle();

  private:
    Servo parallax;
    long angle, target;
    int theta, thetaPre;
    //int minPulse, maxPulse;
    volatile unsigned long firstRise, secondRise, fall;
    int turns;
    int pDiff;
    void defaultSettings();
    int maxError;
    int treshold;
    float _p, _i, _d, _b;
    const int unitsFC = 360;
    const int dutyScale = 1;
    const int q2min = unitsFC / 4;
    const int q3max = q2min * 3;
    int calcPid(int diff);
    float errorsum;
    void moveToTarget();
};

#endif
