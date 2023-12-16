#include <string>
#include <iostream>

#include "enigma.h"
#include "rotor.h"
#include "reflector.h"
#include "steckerbrett.h"

std::string rotor_i = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
char notch_i = 'Q';

std::string rotor_ii = "AJDKSIRUXBLHWTMCQGZNPYFVOE";
char notch_ii = 'E';

std::string rotor_iii = "BDFHJLCPRTXVZNYEIWGAKMUSQO";
char notch_iii = 'V';

std::string rotor_iv = "ESOVPZJAYQUIRHXLNFTGKDCMWB";
char notch_iv = 'J';

std::string rotor_v = "VZBRGITYUPSDNHLXAWMJQOFECK";
char notch_v = 'Z';

Enigma::Enigma(const Steckerbrett& steckerbrett, const Reflector& reflector) :
    _steckerbrett(steckerbrett),
    _rotor_left("config/rotor_left.txt"), _rotor_middle("config/rotor_middle.txt"),
    _rotor_right("config/rotor_right.txt"),
    _reflector(reflector)
{

}

std::string Enigma::Encrypt(std::string initial_string)
{
    std::string encrypted_string;
    char symbol;

    for (std::size_t i = 0; i < initial_string.size(); ++i)
    {
        symbol = initial_string[i];

        symbol = this->_steckerbrett.Encrypt(symbol);
        RotateRotors();

        // std::cout << "Rotor positions: " << rotor_left[rotor_left.GetCurrentPos()]
        //           << ' ' << rotor_middle[rotor_middle.GetCurrentPos()]
        //           << ' ' << rotor_right[rotor_right.GetCurrentPos()] << std::endl;

        symbol = EncryptRotorRight(symbol);
        symbol = EncryptRotorMiddle(symbol);
        symbol = EncryptRotorLeft(symbol);
        symbol = this->_reflector.Reflect(module_div(symbol - INDEX - 
                 this->_rotor_left.GetCurrentPos(), MOD) + INDEX);
        symbol = EncryptRotorLeftBack(symbol);
        symbol = EncryptRotorMiddleBack(symbol);
        symbol = EncryptRotorRightBack(symbol);
        symbol = this->_steckerbrett.Encrypt(module_div((symbol - INDEX - 
                 this->_rotor_right.GetCurrentPos()), MOD) + INDEX);

        encrypted_string += symbol;
    }

    return encrypted_string;
}

void Enigma::RotateRotors()
{
    if (this->_rotor_right[this->_rotor_right.GetCurrentPos()] == 
        this->_rotor_right.GetNotch())
    {

        if (this->_rotor_middle[this->_rotor_middle.GetCurrentPos()] == 
            this->_rotor_middle.GetNotch())
        {
            ++this->_rotor_left;
        }

        ++this->_rotor_middle;
    }

    ++this->_rotor_right;
}

char Enigma::EncryptRotorRight(char symbol)
{
    symbol -= INDEX;
    char new_pos = module_div(((symbol + this->_rotor_right.GetCurrentPos())), MOD);

    return this->_rotor_right[new_pos];
}

char Enigma::EncryptRotorMiddle(char symbol)
{
    symbol -= INDEX;
    char new_pos = module_div((symbol + (this->_rotor_middle.GetCurrentPos() - 
                   this->_rotor_right.GetCurrentPos())), MOD);

    return this->_rotor_middle[new_pos];
}

char Enigma::EncryptRotorLeft(char symbol)
{
    symbol -= INDEX;
    char new_pos = module_div((symbol + (this->_rotor_left.GetCurrentPos() - 
                   this->_rotor_middle.GetCurrentPos())), MOD);

    return this->_rotor_left[new_pos];
}

char Enigma::EncryptRotorRightBack(char symbol)
{
    symbol -= INDEX;
    char new_pos = module_div(((symbol - (this->_rotor_middle.GetCurrentPos() - 
                   this->_rotor_right.GetCurrentPos()))), MOD);

    return this->_rotor_right.GetKeyByValue(new_pos);
}

char Enigma::EncryptRotorMiddleBack(char symbol)
{
    symbol -= INDEX;
    char new_pos = module_div((symbol - (this->_rotor_left.GetCurrentPos() - 
                   this->_rotor_middle.GetCurrentPos())), MOD);

    return this->_rotor_middle.GetKeyByValue(new_pos);
}

char Enigma::EncryptRotorLeftBack(char symbol)
{
    symbol -= INDEX;
    char new_pos = module_div((symbol + this->_rotor_left.GetCurrentPos()), MOD);

    return this->_rotor_left.GetKeyByValue(new_pos);
}
