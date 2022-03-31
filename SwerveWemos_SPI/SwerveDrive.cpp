#include "SwerveDrive.h"
#include "fullServo.h"
#include "Arduino.h"
#include <Servo.h>

SwerveDrive::SwerveDrive(byte s_pin, int _angleOffset, byte m_pin) :
  fullServo(s_pin)
{
  angleOffset = _angleOffset;
  reversed = false;
  reverseOffset = 0;

  motor.attach(m_pin);
  motor.writeMicroseconds(1500);
}

void SwerveDrive::input(int a, int m)
{
  //Serial.print("target: ");
  //Serial.print(a);
  calcResponse(a, m);  //update rot and mag

  //call the fullServo functions here (rot)
  int spdOffset = this->move(rot); // returns movement induced by rotating the axis along a fixed gear

  int pulse = mag + spdOffset;
  pulse = map(pulse, -128, 128, 1000, 2000);
  //Serial.println(pulse);
  motor.writeMicroseconds(pulse);
}

void SwerveDrive::calcResponse(int a, int m)
{
  mag = m;
  // only if there is relevant input
  if (abs(mag) > 0)
  {
    // convert the vector to euclidians
    long targetangle = a + angleOffset + reverseOffset; // target
    // left hand turn distance
    int dist = getRelativeAngle(this->getAngle(), targetangle);
//    Serial.print(this->getAngle());
//    Serial.print("\t");
//    Serial.print(targetangle);
//    Serial.print("\t");
//    Serial.println(dist);

    // magnitude flips
    // if further than a quarter turn, reversing would be faster
    
    if (dist > 90 && dist < 270)
    {
      Serial.println("reversing");
      reversed = !reversed;
      reverseOffset = reversed ? 180 : 0;
      // re-calculate targetangle and dist
      targetangle = a + angleOffset + reverseOffset;
      dist = getRelativeAngle(this->getAngle(), targetangle);
    }

    if (dist < 180) rot = this->getAngle() + dist;
    else rot = this->getAngle() - (360 - dist);
  }
  if (reversed) mag *= -1;
}

int SwerveDrive::getLiveMag()
{
  return mag;
}

void SwerveDrive::updateSpeed(int s)
{
  
}

void SwerveDrive::moveMotor(int spd)
{
  motor.write(spd);
}

void SwerveDrive::setAngleOffset(int _angleOffset)
{
  angleOffset = _angleOffset;
}

int SwerveDrive::getRelativeAngle(long a, long b)
{
  return (720 + ((b - a) % 360)) % 360;
}
