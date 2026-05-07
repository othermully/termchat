#ifndef SERVER_H
#define SERVER_H

#include <cstdint>
#include <string>
#include <poll.h>
#include <vector>

namespace net{

struct Client;

class Server{
public:

  int 	      m_fd{};
  std::string m_name{};

  void Start();

  // Accept conn, create client, set state to AUTH
  Client AcceptClient();

  Server(const uint16_t port, std::string& name);

private:

  std::vector<pollfd> pollfds{};
};
} // net
#endif
