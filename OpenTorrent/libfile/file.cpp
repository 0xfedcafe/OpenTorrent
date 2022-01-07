#include "file.hpp"

#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include "debug.h"
#if defined unix || defined __APPLE__
#include <fcntl.h>  // O_RDONLY
#include <sys/stat.h>
#include <unistd.h>  // read
#include <cstring>   // memchr

void opentorrent::input::File::ReadFile() {
  static const auto BUFFER_SIZE = 16 * 1024;
  int fd = open(path_.c_str(), O_RDONLY);
  if (fd == -1) {  // should throw here
#ifdef DEBUG
    spdlog::info("Can't open file {}. Check Permissions\n", path_);
#endif
    throw std::runtime_error("Check Permissions");
  }
  /* Advise the kernel of our access pattern.  */
  posix_fadvise(fd, 0, 0, 1);  // FDADVICE_SEQUENTIAL
  struct stat st;
  fstat(fd, &st);
  data_.resize(st.st_size);

  char buf[BUFFER_SIZE + 1];

  int i = -1;
  while (ssize_t bytes_read = read(fd, buf, BUFFER_SIZE) && i++) {
    if (bytes_read == -1) {  // throw "read failed"
      if (!bytes_read) {
#ifdef DEBUG
        spdlog::info("Can't read file {}\n", path_);
#endif
        data_.clear();
        throw std::runtime_error("Read Failure");
        break;
      }
    }
    std::copy(buf, buf + BUFFER_SIZE, data_.begin() + i * BUFFER_SIZE);
  }
}
#endif

#if defined __WIN32 || defined __CYGWIN__

#include <windows.h>
// Cygwin defines the unix symbols and doesn't define the win32 ones,
// BE CAREFUL. OTOH it does define __CYGWIN__.
void opentorrent::input::File::ReadFile() {
  static const auto BUFFER_SIZE = 16 * 1024;
  int hFile = CreateFile(path_,                  // file to open
                         GENERIC_READ,           // open for reading
                         FILE_SHARE_READ,        // share for reading
                         NULL,                   // default security
                         OPEN_EXISTING,          // existing file only
                         FILE_ATTRIBUTE_NORMAL,  // normal file
                         NULL);                  // no attr. template
  char buf[BUFFER_SIZE + 1];
  int bytes_read = 0;

  int data_size;
  GetFileSizeEx(hFile, &data_size);
  data_.resize(data_size);

  if (hFile == INVALID_HANDLE_VALUE) {
#ifdef DEBUG
    spdlog::info("Wrong Handle Value\n", path_);
#endif
    throw std::runtime_error("failed to open file");
  }

  int i = -1;
  while (ReadFile(hFile, buf, BUFFER_SIZE, &bytes_read, NULL) && i++) {
    if (bytes_read > 0) {
      std::copy(buf, buf + BUFFER_SIZE, data_.begin() + i * BUFFER_SIZE);
    }
  }
  if (CloseHandle(hFile)) {
#ifdef DEBUG
    spdlog::info("Can't close file\n", path_);
#endif
    throw std::runtime_error("failed to close file");
  }
}

#endif
