#ifndef SwerveDrive_h
#define SwerveDrive_h

#include "Arduino.h"
#include "fullServo.h"

#include <Servo.h>

class SwerveDrive : public fullServo
{
  public:
    SwerveDrive(byte _s_pin, int _angleOffset, byte m_pin);
    void input(int a, int m);
    void setAngleOffset(int _angleOffset);
    int getLiveMag();
    void updateSpeed(int s);

  private:
    // variables
    int angleOffset;
    bool reversed;
    int reverseOffset;
    //bool slowMode;
    const int motorNeutral = 90;
    int cx, cy; //rotation point
    int px, py; //physical location
    long rot;
    int mag; // derived from input
    //int reversed;

    // objects
    Servo motor;
    // functions
    //int getRelativeAngle(int a, int b);
    int getRelativeAngle(long a, long b);
    void calcResponse(int a, int m);
    void moveMotor(int spd);
};

#endif
