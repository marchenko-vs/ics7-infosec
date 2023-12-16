#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

#define MOD 26
#define INDEX 65

unsigned int module_div(const int dividend, const unsigned int mod);
std::string read_string(const std::string filename);
int check_string(std::string& initial_string);

#endif // _UTILS_H_
