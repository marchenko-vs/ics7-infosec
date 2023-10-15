class Reflector
{
public:
    Reflector() = default;
    Reflector(const Reflector& reflector);
    Reflector(const std::string filename);
    char Reflect(const char symbol);
private:
    std::vector<std::string> _symbols;
};
