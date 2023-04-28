#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>

class FileRAII {
private:
  int temp_fd;
  char *temp_filename = nullptr;
  FILE *temp_c_file = nullptr;
  std::ofstream *temp_cpp_file = nullptr;

public:
  FileRAII(std::string template_name) {
    temp_filename = (char*)template_name.c_str();
    temp_fd = mkstemp(temp_filename);

    if (temp_fd == -1) {
      std::perror("Could not create temporary file: ");
      std::exit(1);
    }
  }

  ~FileRAII() {
    if (temp_cpp_file != nullptr) {
      temp_cpp_file->close();
      delete temp_cpp_file;
    }

    if (temp_c_file) {
      fclose(temp_c_file);
    }

    close(temp_fd);
    remove(temp_filename);
  }

  int getTempFd() { return temp_fd; }

  std::string getFilename() { return temp_filename; }

  FILE *getFILE(char const *mode = "r") {
    if (!temp_c_file) {
      temp_c_file = fdopen(temp_fd, mode);
    }
    return temp_c_file;
  }

  std::ofstream *getOFStream() {
    if (!temp_cpp_file) {
      temp_cpp_file = new std::ofstream(temp_filename);
    }
    return temp_cpp_file;
  }
};