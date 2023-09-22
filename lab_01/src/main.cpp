#include <iostream>
#include <fstream>

#include "enigma.h"
#include "reflector.h"
#include "utils.h"
#include "steckerbrett.h"

#define ERR_ARGC -1
#define ERR_ASCII -2

int main(int argc, char **argv)
{
    Steckerbrett steckerbrett;
    Reflector reflector;

    std::string in_file, out_file;

    if (argc == 3)
    {
        steckerbrett = Steckerbrett("config/steckerbrett.txt");
        reflector = Reflector("config/reflector.txt");

        in_file = argv[1];
        out_file = argv[2];
    }
    else if (argc == 5)
    {
        steckerbrett = Steckerbrett(argv[1]);
        reflector = Reflector(argv[2]);
        
        in_file = argv[3];
        out_file = argv[4];
    }
    else
    {
        std::cout << "Error: incorrect number of arguments." << std::endl;
        return ERR_ARGC;
    }

    std::string initial_string = read_string(in_file);

    if (check_string(initial_string))
    {
        std::cout << "Error: incorrect symbols in file." << std::endl;
        return ERR_ASCII;
    }

    Enigma enigma = Enigma(steckerbrett, reflector);
    std::string encrypted_string = enigma.Encrypt(initial_string);    
    
    std::ofstream file;
    file.open(out_file);
    file << encrypted_string << std::endl;

    return 0;
}
