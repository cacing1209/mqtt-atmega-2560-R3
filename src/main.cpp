#include <Arduino.h>
#define maxrelay 31
#define maxbyte 5

#define relayoff HIGH
#define relayon LOW

const byte Relay[maxrelay] = {22, 23, 24, 25, 26, 27,
                              28, 29, 30, 31, 32, 33,
                              34, 35, 36, 37, 38, 39,
                              40, 41, 42, 43, 44, 45,
                              46, 47, 48, 49, 50, 51, 52};

enum status_Relay
{
  ON = 0x01,
  OFF = 0x00,
};

struct taskState
{
  byte pinRelay;
  unsigned long TimeOn;
  status_Relay status;
};

taskState task[maxrelay];

void initMypin()
{
  for (size_t i = 0; i < maxrelay; i++)
  {
    task[i].pinRelay = Relay[i];
    task[i].TimeOn = 0;
    task[i].status = OFF;
    pinMode(task[i].pinRelay, OUTPUT);
    digitalWrite(task[i].pinRelay, relayon);
  }
}

bool check_CMD(byte status)
{
  if (((status >> 6) & 0b11) == 0b11 || ((status >> 6) & 0b11) == 0b10)
  {
    return true;
  }

  else
  {
    return false;
  }
}

bool status_CMD(byte cmd)
{
  if (((cmd >> 6) & 0b11) == 0b11)
    return true;
  else
    return false;
}

void main_Relay()
{

  for (size_t i = 0; i < maxrelay; i++)
  {
    const int INTERVAL_RL = 10000;
    // const int INTERVAL_RL = 2000; // SATUAN ML
    if (task[i].status == ON)
    {
      digitalWrite(task[i].pinRelay, relayon);
      // Serial.print("pin" + String(task[i].pinRelay));
      // Serial.print(" time out: " + String((INTERVAL_RL - (millis() - task[i].TimeOn)) / 1000));
      // Serial.print(' ');
      if (millis() - task[i].TimeOn > INTERVAL_RL)
      {
        task[i].TimeOn = millis();
        task[i].status = OFF;
      }
    }
    else
    {
      task[i].TimeOn = millis();
      digitalWrite(task[i].pinRelay, relayoff);
    }
  }
  // Serial.println();
}

void check_data(byte bytPin)
{
  if (!check_CMD(bytPin))
    return;
  byte cmd = bytPin & 0b00011111;

  for (size_t i = 0; i < maxrelay; i++)
  {
    if (status_CMD(bytPin) && i == cmd - 1)
    {
      task[i].status = ON;
    }
  }
}

uint8_t checkRelayON()
{
  byte totalRl = 0;
  for (size_t i = 0; i < maxrelay; i++)
  {
    if (task[i].status == ON)
    {
      totalRl++;
      Serial.print(task[i].pinRelay);
      Serial.print(',');
    }
  }
  Serial.println();
  return totalRl;
}

bool chat_open(byte *xp)
{
  const byte equal[maxbyte] = {0x15, 0x05, 0x00, 0x05, 0x15};

  for (size_t i = 0; i < maxbyte; i++)
  {
    if (i == 2)
      continue;
    if (xp[i] != equal[i])
      return false;
  }
  return true;
}

void esp_message(byte *cmd)
{
  while (Serial3.available() > 0)
  {
    static byte buffer[maxbyte];

    byte b = Serial3.read();

    for (int i = 0; i < maxbyte - 1; i++)
    {
      buffer[i] = buffer[i + 1];
    }
    buffer[maxbyte - 1] = b;

    if (chat_open(buffer))
    {
      *cmd = buffer[2];
      for (size_t i = 0; i < maxbyte; i++)
      {
        Serial.print(buffer[i]);
      }
      Serial.println();
      break;
    }
  }
}

void setup()
{
  Serial3.begin(9600);
  Serial.begin(9600);
  initMypin();
  Serial.println(" Device Ready ");
}

void loop()
{
  byte cmd = 0;
  esp_message(&cmd);
  check_data(cmd);
  main_Relay();
}

// void loop()
// {

//   static int incomingByte = 0;
//   byte cmd = 0;
//   while (Serial.available() > 0)
//   {
//     incomingByte = Serial.read();

//     cmd = incomingByte;
//     // Serial.print(cmd, DEC);
//     // Serial.print("BINER: ");
//     // Serial.println(cmd, BIN);
//   }

//   master_Relay(cmd);
//   main_Relay();
//   // testSerial(&cmd);
// }
