class Steckerbrett
{
public:
    Steckerbrett() = default;
    Steckerbrett(const Steckerbrett& steckerbrett);
    Steckerbrett(const std::string filename);
    char Encrypt(const char symbol);
private:
    std::vector<std::string> _symbols;
};
