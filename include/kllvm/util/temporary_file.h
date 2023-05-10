#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <unistd.h>

struct deleter {
  void operator()(FILE *fp) const { std::fclose(fp); }
};

class temporary_file {
private:
  int temp_fd;
  std::string temp_filename;
  std::unique_ptr<FILE, deleter> temp_c_file;
  std::optional<std::ofstream> temp_cpp_file;

public:
  temporary_file(std::string template_name)
      : temp_fd(mkstemp(template_name.data()))
      , temp_filename(template_name) {
    if (temp_fd == -1) {
      throw std::runtime_error("Could not create temporary file!");
    }
  }

  ~temporary_file() {
    close(temp_fd);
    remove(temp_filename.data());
  }

  int descriptor() const { return temp_fd; }

  std::string const &filename() const { return temp_filename; }

  FILE *file_pointer(std::string const &mode = "r") {
    if (!temp_c_file) {
      auto f = fdopen(temp_fd, mode.data());
      if (f) {
        temp_c_file = std::unique_ptr<FILE, deleter>(f);
      } else {
        throw std::runtime_error(
            "Could not open file " + temp_filename + ": "
            + std::strerror(errno));
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

  void rename(std::string const &new_name) {
    std::rename(temp_filename.data(), new_name.data());
  }
};
