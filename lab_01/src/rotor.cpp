#include <string>
#include <fstream>
#include <iostream>

#include "rotor.h"

Rotor::Rotor() : _symbols(nullptr), _notch(0), _current_pos(0)
{

}

Rotor::Rotor(std::string filename)
{
    std::ifstream file;
    file.open(filename);

    std::getline(file, this->_symbols);
    file >> this->_notch;
    file >> this->_current_pos;

    for (std::size_t i = 0; i < this->_symbols.size(); ++i)
    {
        if (this->_current_pos == this->_symbols[i])
            this->_current_pos = i;

    }
}

Rotor& Rotor::operator ++ ()
{
    ++this->_current_pos;

    if (this->_current_pos >= MOD)
        this->_current_pos = 0;

    return (*this);
}

char Rotor::GetKeyByValue(const char index)
{
    const char symbol = this->_alphabet[index];

    for (std::size_t i = 0; i < this->_symbols.size(); ++i)
        if (symbol == this->_symbols[i])
            return this->_alphabet[i];

    return 0;
}

char Rotor::operator [] (const char index)
{
    return this->_symbols[index];
} 

char Rotor::GetCurrentPos()
{
    return this->_current_pos;
}

char Rotor::GetNotch()
{
    return this->_notch;
}
