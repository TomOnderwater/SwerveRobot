#include "SwerveDrive.h"
#include <SimpleFOC.h>
#include "Smoother.h"
#include "SPISlave.h"

#define SERVOPIN    D4
#define MOTORPIN    D3

#define FEEDBACKPIN A0

MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);

SwerveDrive module = SwerveDrive(SERVOPIN, 0, MOTORPIN);

unsigned long timer;

int _a = 0, _m = 0;

Smoother anglespeed;

volatile int count = 0;

IRAM_ATTR void isr()
{
  count ++;
}

struct datapackage
{
  int16_t a;
  int16_t m;
  //uint8_t s;
};

datapackage masterdata;

void Encode(struct datapackage d, uint8_t * buff)
{
  size_t s = sizeof(datapackage);
  memcpy(buff, &d, s);
}

struct datapackage Decode(uint8_t * data)
{
  datapackage d;
  size_t s = sizeof(datapackage);
  memcpy(&d, data, s);
  return d;
}

int speedNow()
{
  int c = count;
  count = 0;
  return c;
}

void updateModule()
{
  sensor.update();
  float a = sensor.getAngle();
  float v = anglespeed.calcSpeed(a);
  module.updateAngle(a, v);
}

void setupModule()
{
  module.setAccuracy(1);
  pinMode(FEEDBACKPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(FEEDBACKPIN), isr, RISING);
}


void setup() {

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  timer = millis();

  // configure i2C
  Wire.setClock(400000);
  // initialise magnetic sensor hardware
  sensor.init();

  delay(100);

SPISlave.onData([](uint8_t * data, size_t len) {
    masterdata = Decode(data);
    uint8_t buff[32];
    //Encode(masterdata, buff);
    //SPISlave.setData((char *) buff);
  });

  SPISlave.begin();
  SPISlave.setStatus(0);
  masterdata.a = 0;
  masterdata.m = 0;

  setupModule();
}

void loop() {
  updateLoop();
}

void updateLoop()
{
  updateModule();
  if (millis() > timer + 10) //update loop
  {
    int s = speedNow();
    
    //Serial.println(s);
    Serial.print(masterdata.a);
    Serial.print(",");
    Serial.println(masterdata.m);
    module.updateSpeed(s);
    //Serial.println(module.getAngle());
    module.input((int)masterdata.a, (int)masterdata.m);
    //module.input(_a, 1);
    timer = millis();
  }
}


int upgradeAngle(int a)
{
  return ((a % 360) + 720) % 360;
}

int radToDeg(float a)
{
  return round(a * 57.2957f);
}
