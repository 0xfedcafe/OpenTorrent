#ifndef OPENTORRENT_FILE_H
#define OPENTORRENT_FILE_H
#include <string>

namespace opentorrent {
namespace input {
class File {
 public:
  void ReadFile();

 private:
  std::string data_;
  std::string name_;
  std::string path_;
};
};  // namespace input
};  // namespace opentorrent

#endif
