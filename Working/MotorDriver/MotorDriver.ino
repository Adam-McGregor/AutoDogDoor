#include <Tic.h>

#define SLAVE_ADDRESS 0x04
#define MOTOR_ADDRESS 0x0e
#define DIGITS 3
#define MAX_HEIGHT 700
#define MIN_HEIGHT 0

TicI2C tic;
int number = 0;
char num[DIGITS+1]; // character representation of number
int i = 0; //i = counter
bool target_reached = false; //has the target been reached
bool set = false; //Check whether the target has been set

void setup()
{
  // Set up I2C.
  Wire.begin(MOTOR_ADDRESS);
  delay(100);
  Wire.begin(SLAVE_ADDRESS);
  delay(100);
  Serial.begin(9600);
  delay(100);
  
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  delay(100);
  Serial.println("Ready!");
  
  tic.haltAndSetPosition(0);
  tic.exitSafeStart();
}

void loop()
{
  tic.resetCommandTimeout();
  if (number == tic.getCurrentPosition())
  {
    target_reached = true;
  }
  else if (!set)
  {
    Serial.print("Setting Target: ");
    Serial.println(number);
    tic.setTargetPosition(number);
    set = true;
    target_reached = false;
  }
  else
  {
    target_reached = false;
  }
  tic.resetCommandTimeout();
  delay(100);
}

void targetInRange()
{
  if (number > 700)
    {
      Serial.println("Out of Range: Reseting Target");
      number = MAX_HEIGHT;
      Serial.print("Target: ");
      Serial.println(number);
    }
    else if (number < 0)
    {
      Serial.println("Out of Range: Reseting Target");
      number = MIN_HEIGHT;
      Serial.print("Target: ");
      Serial.println(number);
    }
}

void setTarget()
{
  number = 0;
  for (int j = 0; j < DIGITS; j++)
  {
    number += ( (int)num[j] * ceil(pow(10,(DIGITS - 1 - j))) );
  }
  Serial.print("Target: ");
  Serial.println(number);
  targetInRange();
  set = false;
  target_reached = false; //new target
}

void receiveData(int byteCount)
{
  while(Wire.available())
  {
    num[i] = (char)Wire.read();
  }
  if (i == DIGITS - 1)
  {
    setTarget();
    i = 0; //reset
  }
  else
  {
    i++; //next number
  }
}

void sendData()
{
  Serial.print("data sent: ");
  Serial.println(target_reached);
  Wire.write( target_reached );
}
