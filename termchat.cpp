#include <exception>
#include <iostream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <poll.h>
#include <fcntl.h>

int SetNonBlocking(int fd){
  int flags = fcntl(fd, F_GETFL, 0); 
  if (flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK); 
}

namespace chat{}; // chat

namespace core{}; // core

namespace net{
class Server{
public:

  // Constructor
  Server(){
    // socket setup
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    m_fd_             = server_socket;
    SetNonBlocking(server_socket);


    // address setup
    sockaddr_in server_addr{};
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(9001);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t len               = sizeof(server_addr);

    // bind
    if (bind(server_socket, (sockaddr*)&server_addr, len) < 0) {
      perror("Bind error");
      exit(1);
    }

    // listen
    if (listen(server_socket, SOMAXCONN) < 0) {
      perror("Listen error");
      exit(1);
    }

    // pfds
    pollfd pfd{};
    pfd.fd     = server_socket;
    pfd.events = POLLIN;
    m_pfds_.push_back(pfd);

    std::cout << "Listening on port 9001" << std::endl;
  }

  // Main event loop
  void Start(){
    int ret;
    int timeout_msecs = 500;

    while (true) {
      ret = poll(m_pfds_.data(), m_pfds_.size(), timeout_msecs);
      if (ret <= 0) continue;

      for (size_t i = 0; i < m_pfds_.size(); ++i) {
        if (m_pfds_[i].revents & POLLIN) {
          if (m_pfds_[i].fd == m_fd_) {
            // New connection
          }

          else {
            // Existing connection
          }
        
        }
      }
    } // while (true)
  }

private:
  int                 m_fd_{};
  std::vector<pollfd> m_pfds_{};

};
}; // net

int main(){

  return 0;
}
