
#include <string>
#include <dirent.h>
#include <spdlog/spdlog-inl.h>

/**
 * Return the current working dircetory
 * @return a std::string with the path to the CWD
 */
std::string get_cwd() {
  char temp[PATH_MAX];
  return ( getcwd(temp, PATH_MAX) ? std::string( temp ) : std::string("") );
}

/**
 * Return the list of files in a directory which meet a specific criterion
 * @param directory The directory to search
 * @param files A vector to be filled by file names meeting the filter
 * @param filter The function which filters the files. Should return true if a file name matches the filter.
 */
void files_in_directory(const std::string &directory, std::vector<std::string> &files,
                        const std::function<bool(const char * const)> &filter) {
  files.clear();

  DIR *dir = opendir(directory.c_str());
  if (dir == nullptr) {
    spdlog::error("Problem reading directory {}", directory);
    return;
  }

  struct dirent *ent;
  while ((ent = readdir(dir)) != nullptr) {
    // Call the filter with each
    if (filter(ent->d_name)) {
      // Add matching file name
      files.emplace_back(ent->d_name);
    }
  }
  closedir(dir);
}
