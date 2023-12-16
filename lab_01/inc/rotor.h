#ifndef _ROTOR_H_
#define _ROTOR_H_

#include "utils.h"

class Rotor
{
private:
    const std::string _alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string _symbols;
    char _notch;
    char _current_pos;

public:
    Rotor();
    Rotor(std::string filename);
    
    Rotor& operator ++ ();
    char GetKeyByValue(const char index);
    char operator [] (const char index);
    char GetCurrentPos();
    char GetNotch();
};

#endif // _ROTOR_H_
