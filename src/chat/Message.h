#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>

namespace chat{
struct Message{
  std::string 		   m_prefix{};
  std::vector<std::string> m_args{};
  std::string 		   m_trailing{};
};
} // chat


#endif
