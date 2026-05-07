#ifndef CLIENT_H
#define CLIENT_H

#include <string>

namespace net{

enum class ClientState{
  AUTH,
  REGISTERING,
  REGISTERED
};

struct Client{
  int         m_fd{};
  std::string m_nickname{};
  std::string m_username{};

  std::string m_input_buffer{};
  std::string m_output_buffer{};

  ClientState state{};
};
} // net

#endif
