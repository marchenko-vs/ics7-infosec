class Rotor
{
public:
    Rotor();
    Rotor(const std::string symbols, const char notch, const char current_pos);
    Rotor& operator ++ ();
    char GetKeyByValue(const char index);
    char operator [] (const char index);
    char GetCurrentPos();
    char GetNotch();
private:
    const std::string _alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string _symbols;
    char _notch;
    char _current_pos;
};
