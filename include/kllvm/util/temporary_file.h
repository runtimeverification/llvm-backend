#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <unistd.h>

typedef std::unique_ptr<FILE, int (*)(FILE *)> temp_c_file_type;

class temporary_file {
private:
  int temp_fd;
  std::string temp_filename;
  temp_c_file_type temp_c_file;
  std::optional<std::ofstream> temp_cpp_file;

public:
  temporary_file(std::string template_name)
      : temp_fd(mkstemp(template_name.data()))
      , temp_filename(template_name)
      , temp_c_file(nullptr, &fclose) {
    if (temp_fd == -1) {
      std::runtime_error("Could not create temporary file!");
    }
  }

  ~temporary_file() {
    if (!std::filesystem::remove(temp_filename)) {
      std::runtime_error("Could not delete temporary file!");
    }
  }

  int descriptor() const { return temp_fd; }

  std::string const &filename() const { return temp_filename; }

  FILE *file_pointer(std::string const &mode = "r") {
    if (!temp_c_file.get()) {
      auto f = fdopen(temp_fd, mode.data());
      if (f) {
        temp_c_file_type(f, [](std::FILE *p) { return fclose(p); });
      } else {
        std::runtime_error("Could not open file " + temp_filename);
      }
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