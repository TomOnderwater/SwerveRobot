#include "SwerveController.h"

SwerveController::SwerveController(float _x, float _y, int _r, uint8_t _adr)
{
  x = _x;
  y = _y;
  r = _r;
  rs = 0.01f;
  s = 0.3f;
  address = _adr;
  mag = 0;
  angle = 0;  
  this->setRotationCenter(0.0f, 0.0f);
}

uint8_t SwerveController::getAddress()
{
  return address;
}
void SwerveController::update(int _x, int _y, int _r)
{
  float xmag = ((float)_x * s) + ((float)_r * xRotMultiplier * rs);
  float ymag = ((float)_y * s) + ((float)_r * yRotMultiplier * rs);
  
  mag =  round(sqrt(sq(xmag) + sq(ymag)));
  angle = upgradeAngle(radToDeg(atan2(xmag, ymag)));
}

int SwerveController::getAngle()
{
  return angle + r;
}

int SwerveController::getMag()
{
  return mag;
}

void SwerveController::setSpeed(float _s)
{
  s = _s;
}
void SwerveController::setRotationCenter(float _rx, float _ry)
{
  xRotMultiplier = (y - _ry);
  yRotMultiplier = -(x - _rx);
}

void SwerveController::setRotationSpeed(float _rs)
{
  rs = _rs;
}

int SwerveController::upgradeAngle(int a)
{
  return (a + 720) % 360;
}

int SwerveController::radToDeg(float a)
{
  return round(a * 57.2957f);
}
