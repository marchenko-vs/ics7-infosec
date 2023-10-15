class Enigma
{
public:
    Enigma(const Steckerbrett& steckerbrett, const Reflector& _reflector);
    std::string Encrypt(std::string initial_string);
    void RotateRotors();
    char EncryptRotorRight(char symbol);
    char EncryptRotorMiddle(char symbol);
    char EncryptRotorLeft(char symbol);
    char EncryptRotorRightBack(char symbol);
    char EncryptRotorMiddleBack(char symbol);
    char EncryptRotorLeftBack(char symbol);
private:
    Steckerbrett _steckerbrett;
    Rotor _rotor_left;
    Rotor _rotor_middle;
    Rotor _rotor_right;
    Reflector _reflector;
};
