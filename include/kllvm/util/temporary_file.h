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
  int temp_fd_;
  std::string temp_filename_;
  std::unique_ptr<FILE, deleter> temp_c_file_;
  std::optional<std::ofstream> temp_cpp_file_;

public:
  temporary_file(std::string template_name)
      : temp_fd_(mkstemp(template_name.data()))
      , temp_filename_(template_name) {
    if (temp_fd_ == -1) {
      throw std::runtime_error("Could not create temporary file!");
    }
  }

  temporary_file(temporary_file const &other) = delete;
  temporary_file &operator=(temporary_file const &other) = delete;

  temporary_file(temporary_file &&other) = delete;
  temporary_file &operator=(temporary_file &&other) = delete;

  ~temporary_file() {
    close(temp_fd_);
    remove(temp_filename_.data());
  }

  int descriptor() const { return temp_fd_; }

  std::string const &filename() const { return temp_filename_; }

  FILE *file_pointer(std::string const &mode = "r") {
    if (!temp_c_file_) {
      auto *f = fdopen(temp_fd_, mode.data());
      if (f) {
        temp_c_file_ = std::unique_ptr<FILE, deleter>(f);
      } else {
        throw std::runtime_error(
            "Could not open file " + temp_filename_ + ": "
            + std::strerror(errno));
      }
    }

    return temp_c_file_.get();
  }

  std::ofstream &ofstream() {
    if (!temp_cpp_file_) {
      temp_cpp_file_ = std::ofstream(temp_filename_);
    }
    return temp_cpp_file_.value();
  }

  void rename(std::string const &new_name) {
    std::rename(temp_filename_.data(), new_name.data());
  }
};
