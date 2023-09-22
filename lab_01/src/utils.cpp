#include <string>
#include <fstream>
#include <iostream>

#include "utils.h"

unsigned int module_div(const int dividend, const unsigned int mod)
{
    unsigned int result = 0;

    if (dividend >= 0)
        result = dividend % mod;
    else
        result = mod + dividend;

    return result; 
}

std::string read_string(const std::string filename)
{
    std::ifstream file;
    file.open(filename);
    std::string result, tmp;

    while(std::getline(file, tmp))
    {
        result += tmp;
    }

    return result;
}

int check_string(std::string& initial_string)
{
    int rc = 0;

    for (std::size_t i = 0; rc == 0 && i < initial_string.size(); ++i)
    {
        if (initial_string[i] >= 97 && initial_string[i] <= 122)
        {
            initial_string[i] -= 32;
        }
        else if (initial_string[i] > 90 || initial_string[i] < 65)
        {
            rc = -1;
        }
    }

    return rc;
}
