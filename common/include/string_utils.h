//
// Created by Dave Durbin on 17/3/2023.
//

#ifndef SHADY_STRING_UTILS_H
#define SHADY_STRING_UTILS_H

#include <vector>
#include <string>

/* split a string into substrings using delimiter */
std::vector <std::string> split(const std::string &line, char delimiter);

/* trim from start in place */
void ltrim(std::string &s);

/* trim from end in place */
void rtrim(std::string &s);

/* trim both ends place */
void trim(std::string &s);
#endif //SHADY_STRING_UTILS_H
