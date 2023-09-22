#include <string>
#include <fstream>
#include <vector>

#include "reflector.h"
#include "rotor.h"

Reflector::Reflector(const Reflector& reflector)
{
    this->_symbols = reflector._symbols;
}

Reflector::Reflector(const std::string filename)
{
    std::ifstream file;
    file.open(filename);
    std::string tmp;

    while (std::getline(file, tmp))
    {
        this->_symbols.push_back(tmp);
    }
}

char Reflector::Reflect(const char symbol)
{
    for (std::size_t i = 0; i < this->_symbols.size(); ++i)
    {
        if (this->_symbols[i][0] == symbol)
            return this->_symbols[i][1];

        if (this->_symbols[i][1] == symbol)
            return this->_symbols[i][0];
    }

    return symbol;
}
