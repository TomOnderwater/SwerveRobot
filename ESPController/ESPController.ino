#include "SwerveController.h"
#include "SPIMaster.h"

// **** choose ONE input method ****
#define PS4INPUT // esp32 exclusive
//#define APINPUT // connect to wifi and browse to: 192.168.4.1 on your phone

#define DAMPING 0.15f //smoothing of speed control, lower is more damping

#ifdef ESP8266
#include "wemos_pcb.h"
#endif

#ifdef ESP32
#include "esp32_pcb.h"
#endif

#ifdef PS4INPUT
#include <PS4Controller.h>
#include "ButtonMaker.h"
char * mac = "14:C2:13:14:9C:7D"; // replace with the mac stored in your PS4 controller
ButtonMaker left, right, up, down;
#endif

#ifdef APINPUT
#include <WiFiController.h> // https://github.com/TomOnderwater/WiFiController
WiFiController * master = NULL;
const char* ssid = "OmniBot";
const char* password = "123456789";
#endif

SPIMaster spi;

#define SWERVECOUNT 3
SwerveController swerve[] =
{
  SwerveController(15.0f, 10.0f, 101, CS_C),
  SwerveController(-15.0f, 10.0f, -57, CS_D),
  SwerveController(0.0f, -20.0f, 15, CS_B)
};

unsigned long timer;

struct controls {
  int y, x, r;
};

float rotx, roty;

int rotationOffset = 0;

const float s_fraction = 0.7f / 255.0f;
float sprintMultiplier = 1.0f;

struct datapackage
{
  int16_t a;
  int16_t m;
  //uint8_t s;
};

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

struct controls target;
struct controls input;

void setup() {
  Serial.begin(115200);

  //spi hardware init
  for (int i = 0; i < SWERVECOUNT; i++)
  {
    pinMode(swerve[i].getAddress(), OUTPUT);
    digitalWrite(swerve[i].getAddress(), HIGH);
  }

#ifdef APINPUT
  master = new WiFiController(ssid, password);
  Serial.print("IP: ");
  Serial.println(master->getIP());
#endif

#ifdef PS4INPUT
  PS4.begin(mac);
#endif

  zeroInput();
  timer = millis();

  //update rotation center
  rotx = 0.0f;
  roty = -5.0f;
  setRotationCenter(rotx, roty);

  setRotationSpeed(0.008f);

  delay(500);
  // point wheels forward
  target.y = 5;
  updateBot();
  delay(500);
  zeroInput();
  updateBot();
}

void loop() {

#ifdef APINPUT
  master->update();
  if (master->connected()) input = updateOverAP();
  else zeroInput();
#endif
#ifdef PS4INPUT
  if (PS4.isConnected()) input = updateByPS4();
  else zeroInput();
#endif
  updateBot();
}

void setRotationSpeed(float rs)
{
  for (int i = 0; i < SWERVECOUNT; i++)
  {
    swerve[i].setRotationSpeed(rs);
  }
}

void setRotationCenter(float x, float y)
{
  for (int i = 0; i < SWERVECOUNT; i++)
  {
    swerve[i].setRotationCenter(x, y);
  }
}


void updateBot()
{
  if (millis() > timer + 20)
  {
    // update target (damping for physical movement)
    target.x += DAMPING * (input.x - target.x);
    target.y += DAMPING * (input.y - target.y);
    target.r += DAMPING * 2 * ((input.r + rotationOffset) - target.r);

    //    Serial.print(target.x);
    //    Serial.print(" - ");
    //    Serial.print(target.y);
    //    Serial.print(" - ");
    //    Serial.println(target.r);

    for (int i = 0; i < SWERVECOUNT; i++)
    {
      auto s = swerve[i];
      s.setSpeed(sprintMultiplier);
      s.update(target.x, target.y, target.r);
      datapackage data;
      data.a = (int16_t) s.getAngle(); // + 360; //weird trick to make the data connection work (???)
      data.m = (int16_t) s.getMag();

      uint8_t buff[32]; //buffer
      Encode(data, buff);
      spi.sendData(buff, sizeof(datapackage), s.getAddress());
    }
    timer = millis();
  }
}

void zeroInput()
{
  input.x = 0;
  input.y = 0;
  input.r = 0;
  target.x = 0;
  target.y = 0;
  target.r = 0;
}
#ifdef APINPUT
struct controls updateOverAP() {
  struct controls res;

  res.x = master->x1();
  res.y = master->y1();
  res.r = master->x2();

  return res;
}
#endif

#ifdef PS4INPUT
struct controls updateByPS4() {
  struct controls res;

  res.x = 0;
  res.y = 0;
  res.r = 0;


  // update here

  // translation
  if (abs(PS4.LStickX()) > 10) res.x = PS4.LStickX();
  if (abs(PS4.LStickY()) > 10) res.y = PS4.LStickY();

  // rotation
  if (abs(PS4.RStickX()) > 10) res.r = PS4.RStickX();

  //Serial.println(PS4.R2Value());
  sprintMultiplier = ((float)PS4.R2Value() * s_fraction) + 0.3f;

  if (up.isPress(PS4.Up()))
  {
    roty += 5.0f;
    setRotationCenter(rotx, roty);
  }
  if (down.isPress(PS4.Down()))
  {
    roty -= 5.0f;
    setRotationCenter(rotx, roty);
  }

  if (right.isPress(PS4.Right())) rotationOffset ++;
  if (left.isPress(PS4.Left())) rotationOffset --;

  return res;
}
#endif


/*
 * // no idea if this is allowed
 * void * functions [] = {
 * [&]() {
 *  timeA = micros();
 *  count ++;
 * }, [&](){...}
 * , ... 8
 * }
 * //win cases are 4, 1, 6 or 0, 1, 4 never progress count beyond 3 (end condition)
 * //cases where count = 3 should end without an if statement
 * // (max 7)
 * // functions[(count << 1) | state](); //execute function in memory to bypass branching
 * 
 * //alternative (max 7)
 * mycounts[((count++) | state << 2) - 1] = micros();
 * count ^= B11111000
 * falling first = 000 = 0 -1 = -1 // NOT
 * rising first = 1 = 0
 * rising second = 011 = 3 - 1 = 2 // NOT
 * falling second 010 = 2 = 1
 * rising third 101 = 5 - 1 = 4
 * falling third 100 = 4 - 1 = 3   // NOT
 * falling fourth 110 = 6
 * rising fourth 111 = 7
 * 
 * falling fifth = 1000 = 8
 * rising fifth = 1001 = 9
 */
