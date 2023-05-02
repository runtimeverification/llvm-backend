#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <unistd.h>

typedef std::unique_ptr<FILE, decltype(&fclose)> temp_c_file_type;

class temporary_file {
private:
  int temp_fd;
  std::string temp_filename;
  temp_c_file_type temp_c_file;
  std::optional<std::ofstream> temp_cpp_file;

public:
  temporary_file(std::string template_name)
      : temp_c_file(nullptr, &fclose) {
    temp_fd = mkstemp(template_name.data());
    temp_filename = template_name;

    if (temp_fd == -1) {
      std::runtime_error("Could not create temporary file!");
    }
  }

  ~temporary_file() {
    close(temp_fd);
    remove(temp_filename.data());
  }

  int descriptor() { return temp_fd; }

  std::string filename() { return temp_filename; }

  FILE *file_pointer(std::string const &mode = "r") {
    if (!temp_c_file.get()) {
      temp_c_file = temp_c_file_type(fdopen(temp_fd, mode.data()), &fclose);
    }
    return temp_c_file.get();
  }

  std::ofstream &ofstream() {
    if (!temp_cpp_file) {
      temp_cpp_file = std::ofstream(temp_filename);
    }
    return temp_cpp_file.value();
  }
};