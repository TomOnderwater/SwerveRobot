#ifndef SwerveController_h
#define SwerveController_h

#include "Arduino.h"

class SwerveController
{
  public:
  SwerveController(float _x, float _y, int _r, uint8_t _adr);
  void update(int _x, int _y, int _r);
  int getAngle();
  int getMag();
  void setRotationSpeed(float _rs);
  void setRotationCenter(float _rx, float _ry);
  void setSpeed(float _s);
  uint8_t getAddress();

  private:
  float x, y;
  float xRotMultiplier, yRotMultiplier;
  float rs, s;
  uint8_t address;
  int radToDeg(float a);
  int upgradeAngle(int a);
  int mag, angle;
  int r;
};

#endif
