#ifndef _REFLECTOR_H_
#define _REFLECTOR_H_

#include "rotor.h"

#include <vector>

class Reflector
{
private:
    std::vector<std::string> _symbols;

public:
    Reflector() = default;
    Reflector(const Reflector& reflector);
    Reflector(const std::string filename);

    char Reflect(const char symbol);
};

#endif // _REFLECTOR_H_
