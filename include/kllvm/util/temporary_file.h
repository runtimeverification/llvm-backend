#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>

class temporary_file {
private:
  int temp_fd;
  std::string temp_filename;
  FILE *temp_c_file = nullptr;
  std::ofstream *temp_cpp_file = nullptr;

public:
  temporary_file(std::string template_name) {
    temp_fd = mkstemp(template_name.data());
    temp_filename = template_name;

    if (temp_fd == -1) {
      std::runtime_error("Could not create temporary file!");
    }
  }

  ~temporary_file() {
    if (temp_cpp_file != nullptr) {
      temp_cpp_file->close();
      delete temp_cpp_file;
    }

    if (temp_c_file) {
      fclose(temp_c_file);
    }

    close(temp_fd);
    remove(temp_filename.data());
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