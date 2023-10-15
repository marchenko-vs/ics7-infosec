#ifndef _STECKERBRETT_H_
#define _STECKERBRETT_H_

#include <string>
#include <fstream>
#include <vector>

class Steckerbrett
{
private:
    std::vector<std::string> _symbols;

public:
    Steckerbrett() = default;
    Steckerbrett(const Steckerbrett& steckerbrett);
    Steckerbrett(const std::string filename);

    char Encrypt(const char symbol);
};

#endif // _STECKERBRETT_H_
