#ifndef Smoother_h
#define Smoother_h

#include "Arduino.h"

class Smoother
{
  public:
  Smoother();
  float calcSpeed(float a);
  void log(float a);
  
  
  private:
  const int _count = 10;
  unsigned long times[10];
  float history[10];
  int index;
  void empty();
};

#endif
