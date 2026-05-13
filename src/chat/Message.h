#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>

namespace chat{
struct Message{
  std::string command{};
  std::string chat_message{};
};
} // chat


#endif
