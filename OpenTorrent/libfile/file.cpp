#include "file.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

#ifdef unix || __APPLE__
#include <fcntl.h>  // O_RDONLY
#include <sys/stat.h>
#include <unistd.h>  // read
#include <cstring>   // memchr

void opentorrent::input::File::ReadFile() {
  static const auto BUFFER_SIZE = 16 * 1024;
  int fd = open(path_.c_str(), O_RDONLY);
  if (fd == -1)  // should throw here
    /* Advise the kernel of our access pattern.  */
    posix_fadvise(fd, 0, 0, 1);  // FDADVICE_SEQUENTIAL
  struct stat st;
  fstat(fd, &st);
  data_.resize(st.st_size);

  char buf[BUFFER_SIZE + 1];

  int i = -1;
  while (size_t bytes_read = read(fd, buf, BUFFER_SIZE) && i++) {
    if (bytes_read == (size_t)-1)  // throw "read failed"
      if (!bytes_read) break;
    std::copy(buf, buf + BUFFER_SIZE, data_.begin() + i * BUFFER_SIZE);
  }
}
#endif

#ifdef __WIN32

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
    // throw an error here
  }

  int i = -1;
  while (ReadFile(hFile, buf, BUFFER_SIZE, &bytes_read, NULL) && i++) {
    if (bytes_read > 0) {
      std::copy(buf, buf + BUFFER_SIZE, data_.begin() + i * BUFFER_SIZE);
    }
  }
  if (CloseHandle(hFile)) {
    // handle error
  }
}

#endif
