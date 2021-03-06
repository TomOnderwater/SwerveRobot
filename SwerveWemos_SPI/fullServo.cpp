#include "fullServo.h"
#include "Arduino.h"
#include <Servo.h>

fullServo::fullServo(byte s_pin)
{
  parallax.attach(s_pin);
  this->defaultSettings();
}

void fullServo::defaultSettings()
{
  // PID settings
  _p = 3.0f;
  _i = 0.3f;
  _d = 50.0f;

  angleSpeed = 0;
  theta = 0;
  angle = 0;
  target = 0;
  errorsum = 0;
  maxError = 40 * round((1.0f / _i));
  pDiff = 0;
}



int fullServo::getAngleSpeed()
{
  return angleSpeed;
}

int fullServo::getTurns()
{
  return turns;
}

void fullServo::setAccuracy(int _treshold)
{
  treshold = _treshold;
}

void fullServo::updateAngle(float t, float v)
{
  theta = upgradeAngle(radToDeg(-t));
  //Serial.println(theta);
  angleSpeed = -v;

  if ((theta < q2min) && (thetaPre > q3max))
    turns++;
  else if ((thetaPre < q2min) && (theta > q3max))
    turns--;

  if (turns >= 0)
    angle = (turns * unitsFC) + theta;
  else if (turns < 0)
    angle = ((turns + 1) * unitsFC) - (unitsFC - theta);
    
  thetaPre = theta;
}

int fullServo::getLiveAngle()
{
  return theta;
}

long fullServo::getAngle()
{
  return angle;
}

int fullServo::move(long degree)
{
  target = degree;
  moveToTarget();
  // should return the torque generated by turning over a bevel gear
  return 0;
}

int fullServo::upgradeAngle(int a)
{
  return ((a % 360) + 720) % 360;
}

int fullServo::radToDeg(float a)
{
  return round(a * 57.2957f);
}


void fullServo::moveToTarget()
{
  int diff = angle - target;
  //parallax.write(90);
  //parallax.write(90 + calcPid(diff));
  parallax.writeMicroseconds(1500 + calcPid(diff));
}

int fullServo::calcPid(int diff)
{
  //if (abs(diff) < treshold * 3) //if nearly on target
  // {
  if ( (pDiff > 0 && diff <= 0) || (pDiff < 0 && diff >= 0) )
  {
    errorsum = 0;
  }
  if (abs(diff) < treshold) //if really on target
  {
    pDiff = diff;
    errorsum = 0; //stop _i from doing unnecessary stuff
    return 0; //constrain(round(_b * speedAVG * angleSpeed), -90, 90); // LESSER BOOST
  }
  else if (abs(diff) > 180) errorsum = -errorsum;
  //  return constrain(round(_b * speedAVG), -90, 90); //BOOST STOP
  //}
  else
  {
    errorsum += diff;
    //errorsum = constrain(errorsum, -maxError, maxError);
    int response = round(_p * diff);
    response += round(_i * errorsum);
    response += round(_d * angleSpeed);
    //response += round(_b * speedAVG);
    response = constrain(response, -500, 500);
    pDiff = diff;
    return response;
  }
}
void fullServo::write(int val)
{
  parallax.write(val);
}

/*
  void fullServo::feedback(uint8_t state)
  {
  // 1 && 00 -> 4
  // 0 && 01 -> 1
  // 1 && 10 -> 6
  // 1 | 0 && 11 = IGNORE
  // encode state in cycleState for no branch (logical OR)
  uint8_t msk = cycleState | (state << 2);
  switch (msk)
  {
    case 4:
      firstRise = micros();
      cycleState ++;
      break;
    case 1:
      fall = micros();
      cycleState ++;
      break;
    case 6:
      secondRise = micros();
      cycleState ++;
      break;
    default:
      break;
  }
  }
*/
