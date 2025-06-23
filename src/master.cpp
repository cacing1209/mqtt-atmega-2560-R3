#include <commond.h>

#define maxrelay 32
#define maxbyte 5
#define baudrate_CL 9600
#define baudrate_PC 115200
#define led 13
#define relayoff HIGH
#define relayon LOW

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {0x9, 0x8, 0x7, 0x6}; // pin baris
byte colPins[COLS] = {0x5, 0x4, 0x3, 0x2}; // pin kolom
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const byte Relay[maxrelay] = {22, 23, 24, 25, 26, 27,
                              28, 29, 30, 31, 32, 33,
                              34, 35, 36, 37, 38, 39,
                              40, 41, 42, 43, 44, 45,
                              46, 47, 48, 49, 50, 51, 52, 53};

enum status_Relay
{
  ON = 0x01,
  OFF = 0x00,
};

enum WifiEvent
{
  disconnect,
  connect
};

struct comunicationWifi
{
  WifiEvent WifiEvent;
  const byte LED = led;
  status_Relay ledstatus;

  long interval;
  byte msgdisconnect, msgconnect;
  void begin()
  {
    WifiEvent = disconnect;
    pinMode(LED, OUTPUT);
    ledstatus = ON;
    msgconnect = 0xA0;
    msgdisconnect = 0xA1;
    interval = 200;
  }
} wifi_;

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
    const int INTERVAL_RL = 1500;
    if (task[i].status == ON)
    {
      digitalWrite(task[i].pinRelay, relayon);
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
    else if (bytPin == wifi_.msgconnect)
      wifi_.WifiEvent = connect;
    else if (bytPin == wifi_.msgdisconnect)
      wifi_.WifiEvent = disconnect;
  }
}

void total_RelayON()
{
  byte totalRl = 0;
  for (size_t i = 0; i < maxrelay; i++)
  {
    if (task[i].status == ON)
    {
      totalRl++;
      Serial.print(task[i].pinRelay);
      Serial.print(" time: ");
      Serial.print(millis() - task[i].TimeOn);
      Serial.print(" locker : ");
      Serial.print(task[i].pinRelay - 21);
      Serial.print(' ');
    }
  }

  if (totalRl > 0)
    Serial.println(totalRl);
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
        Serial.print(buffer[i], HEX);
      }
      Serial.println(" msg: " + String(buffer[2], BIN));
      break;
    }
  }
}
void mainstateled()
{
  static unsigned long lastTime = 0;
  if (wifi_.ledstatus == ON)
  {
    if (millis() - lastTime > wifi_.interval)
    {
      // Serial.println("wifi s disconnect");
      digitalWrite(wifi_.LED, !(digitalRead(wifi_.LED) == HIGH));
      lastTime = millis();
    }
  }
  else
  {
    lastTime = millis();
    digitalWrite(wifi_.LED, LOW);
  }
}
void wifiDisconnect()
{
  if (wifi_.WifiEvent == disconnect)
    wifi_.ledstatus = ON;
  else
    wifi_.ledstatus = OFF;

  mainstateled();
}

// #define size_Num 3
// const String passed = "*717*";

// void manual_Relay(int NumRelay)
// {
//   if (NumRelay >= maxrelay || NumRelay == 0)
//     return;

//   task[NumRelay - 1].status = ON;
// }

// void checek_Password()
// {

// }

// void Manual_Mode()
// {
//   static char ReadlockerNum[size_Num];
//   static bool saveKeypad = false;
//   char is_keypad = customKeypad.getKey();
//   static byte index = 0;
//   static String passKey = "\0";

//   if (is_keypad)
//   {
//     passKey += is_keypad;
//     if (passKey[0] != '*')
//       passKey = "\0";
//     Serial.println(passKey);
//   }
//   if (passKey == passed)
//     saveKeypad = true;
//   else if (is_keypad == 'C')
//   {
//     saveKeypad = false;
//     index = 0;
//     passKey = "\0";
//   }
//   else if (is_keypad == '#')
//   {
//     String convert;
//     for (size_t i = 0; i < size_Num; i++)
//     {
//       convert += ReadlockerNum[i];
//       ReadlockerNum[i] = '\0';
//     }
//     manual_Relay(convert.toInt());
//     saveKeypad = false;
//     index = 0;
//     convert = "\0";
//     passKey = "\0";
//   }
//   else if (saveKeypad)
//   {
//     if (is_keypad != '\0')
//     {
//       ReadlockerNum[index] = is_keypad;
//       index++;
//     }
//     if (index >= size_Num)
//     {
//       for (size_t i = 0; i < size_Num; i++)
//         ReadlockerNum[i] = '\0';
//       index = 0;
//       saveKeypad = false;
//     }
//   }
// }

const String Password = "*717*";

char Read_keypad(manual_state *manual)
{
  manual->is_keypad = customKeypad.getKey();
  if (manual->is_keypad)
  {
    return manual->is_keypad;
  }
  else
    return '\0';
}

void setRole_Keypad(manual_state *keypad)
{
  switch (keypad->statusPassword)
  {
  case PASSED:
    keypad->Role_Keypad = GetNumberlocker;
    Serial.println("get number locker");
    break;
  case DENIDE:
    keypad->Role_Keypad = GetPassword;
    break;
  default:
    keypad->Role_Keypad = Clear;
    Serial.println("is not set");
    break;
  }
}
bool check_Password(String pass)
{
  for (size_t i = 0; i < pass.length(); i++)
  {
    if (pass[i] != Password[i])
      return false;
  }
  return true;
}
void getPassword(manual_state *obj)
{
  static String isPassword;
  char keypad = Read_keypad(obj);

  if (keypad != '\0')
    isPassword += keypad;
  if (isPassword == Password)
  {
    obj->statusPassword = PASSED;
    isPassword = "";
  }
  else if (keypad == 'C')
    isPassword = "";
  else if (keypad == '*')
    isPassword = "*";
  else if (isPassword[0] != '*' || isPassword[0] == 'C')
    isPassword = "";
  else
    obj->statusPassword = DENIDE;
  Serial.println(isPassword);
  setRole_Keypad(obj);
}

bool checkInput_Locker(String input)
{
  const byte size = 5;
  const char erorlist[size] = {'A', 'B', 'C', 'D', '0'};
  for (size_t i = 0; i < size; i++)
  {
    if (input[i] == erorlist[i] || input.toInt() == 0 || input.toInt() > maxrelay)
      return false;
  }
  return true;
}
void getLockerNumber(manual_state *obj)
{
  static String NumberLocker = "";
  char key = Read_keypad(obj);
  static bool Enter = false;

  if (key == '#')
    Enter = true;
  else if (key != '\0')
  {
    NumberLocker += key;
  }
  if (Enter)
  {
    if (checkInput_Locker(NumberLocker))
    {
      task[NumberLocker.toInt() - 1].status = ON;
    }
    else
    {
      Serial.println(0);
    }
    Enter = false;
    NumberLocker = "";
    obj->Role_Keypad = GetPassword;
    obj->statusPassword = DENIDE;
  }
}

void manual_Locker()
{
  static manual_state manual_Locker;
  switch (manual_Locker.Role_Keypad)
  {
  case GetPassword:
    getPassword(&manual_Locker);
    break;
  case GetNumberlocker:
    getLockerNumber(&manual_Locker);
  default:
    break;
  }
}

void setup()
{
  Serial3.begin(baudrate_CL);
  Serial.begin(baudrate_PC);
  wifi_.begin();
  initMypin();
  Serial.println(" Device Ready ");
}
void loop()
{
  byte cmd = 0;
  esp_message(&cmd);
  check_data(cmd);
  wifiDisconnect();
  main_Relay();
  total_RelayON();
  // Manual_Mode();
  manual_Locker();
}