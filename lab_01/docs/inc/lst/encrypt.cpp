std::string Enigma::Encrypt(std::string initial_string)
{
    std::string encrypted_string;
    char symbol;
    for (std::size_t i = 0; i < initial_string.size(); ++i)
    {
        symbol = initial_string[i];
        symbol = this->_steckerbrett.Encrypt(symbol);
        RotateRotors();
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
