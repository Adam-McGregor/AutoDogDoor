#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int number = 0;
int state = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);

  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  Serial.println("Ready!");
}

void loop()
{
  delay(100);
}

void receiveData(int byteCount)
{
  while(Wire.available())
  {
    number = Wire.read();
    Serial.print("data recevied: ");
    Serial.println(number);

    if (number == 1)
    {
      if (state == 0)
      {
        Serial.println("On");
        state = 1;
      }
      else
      {
        Serial.println("OFF");
        state = 0;
      }
    }
  }
}

void sendData()
{
  Serial.print("data sent: ");
  Serial.println(number);
  Wire.write(number);
}
