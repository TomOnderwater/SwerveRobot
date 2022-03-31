#include "Smoother.h"
#include "Arduino.h"


Smoother::Smoother()
{
  this->empty();
}

float Smoother::calcSpeed(float a)
{
  this->log(a);
  int firstindex = (index + 1) % _count;
  float diffbetween = history[index] - history[firstindex];
  return diffbetween * (1000 / float(times[index] - times[firstindex]));
}

void Smoother::log(float a)
{
  index ++;
  index %= _count;
  times[index] = millis();
  history[index] = a;
}
void Smoother::empty()
{
  for (int i = 0; i < _count; i++)
  {
    times[i] = millis();
    history[i] = 0;
  }
  index = 0;
}
