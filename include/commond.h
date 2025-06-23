#include <Arduino.h>

#include <Keypad.h>
#define size_Num 3

enum status_keypad
{
    GetPassword,
    Clear,
    GetNumberlocker,
};
enum Acces_locker
{
    PASSED,
    DENIDE,
};

struct manual_state
{
    char is_keypad;
    String passwordAccess;
    Acces_locker statusPassword;
    status_keypad Role_Keypad;
    char Read_lockerNum[size_Num];

    manual_state()
    {
        is_keypad = '\0',
        statusPassword = DENIDE,
        Role_Keypad = GetPassword;
    }
};