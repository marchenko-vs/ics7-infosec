#include <string>
#include <fstream>
#include <iostream>

#include "steckerbrett.h"

Steckerbrett::Steckerbrett(const Steckerbrett& steckerbrett)
{
    this->_symbols = steckerbrett._symbols;
}

Steckerbrett::Steckerbrett(const std::string filename)
{
    std::ifstream file;
    file.open(filename);
    std::string tmp;

    while (std::getline(file, tmp))
    {
        this->_symbols.push_back(tmp);
    }
}

char Steckerbrett::Encrypt(const char symbol)
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
