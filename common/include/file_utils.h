#ifndef SHADY_FILE_UTILS_H
#define SHADY_FILE_UTILS_H

#include <string>

void files_in_directory(const std::string &directory, std::vector<std::string> &files,
                        const std::function<bool(const char *)> &filter);

/**
 * Return the current working dircetory
 * @return a std::string with the path to the CWD
 */
std::string get_cwd();

#endif //SHADY_FILE_UTILS_H
