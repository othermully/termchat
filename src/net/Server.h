#ifndef SERVER_H
#define SERVER_H

#include <cstdint>
#include <string>
#include <poll.h>
#include <vector>

#include "../core/ServerState.h"


namespace net{


int setNonBlocking(int fd);


class Server{
public:

  int 	      m_fd{};
  std::string m_name{};

  void Start();

  // Accept conn, create client, set state to AUTH
  int AcceptClient();
  void HandleRead(core::ServerState& state, int fd);

  void CleanupFd(int fd); // Disconnect handler for file descriptors

  Server(const uint16_t port, std::string& name);

private:

  std::vector<pollfd> pollfds{};
};
} // net

#endif
