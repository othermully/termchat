#ifndef CLIENT_H
#define CLIENT_H

#include <string>

namespace core{

enum class ClientState{
  AUTH,
  CONNECTED,
  REGISTERED
};

struct Client{
  int         m_fd{};
  std::string m_nickname{};

  std::string m_input_buffer{};
  std::string m_output_buffer{};

  ClientState state{};

};
} // net

#endif
