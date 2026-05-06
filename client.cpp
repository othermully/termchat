#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>

[[maybe_unused]] void Authenticate();
[[maybe_unused]] void Run();

int main(){

  std::vector<pollfd> pfds{};


  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) {
    throw std::runtime_error("client socket error");
    exit(1);
  }

  // Set client_socket to non blocking
  fcntl(client_socket, O_NONBLOCK); 

  struct sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9001);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  socklen_t len = sizeof(server_addr);

  if (connect(client_socket, (sockaddr*)&server_addr, len) < 0) {
    throw std::runtime_error("connect error, is the server running?");
    exit(1);
  }

  std::cout << "Successfully connected to server.\n";

  char buffer[1024] = { '\0' };
  ssize_t r = recv(client_socket, buffer, sizeof(buffer), 0);
  if (r < 0) {
    throw std::runtime_error("recv");
    exit(1);
  }

  std::cout << buffer << std::endl;

  std::string nickname{};
  std::getline(std::cin, nickname);
  send(client_socket, nickname.c_str(), sizeof(nickname), 0);

  struct pollfd client_poll;
  client_poll.fd = client_socket;
  client_poll.events = POLLIN;
  pfds.push_back(client_poll);

  int timeout_msec = 500;
  while (true) {

    int ret = poll(pfds.data(), pfds.size(), timeout_msec);
    if (ret <= 0) continue;

    for (size_t i = 0; i < pfds.size(); ++i) {
      if (pfds[i].revents & POLLIN) {
        char buffer2[1024] = { '\0' };
        recv(client_socket, buffer2, sizeof(buffer2), 0);
        std::cout << buffer2 << std::endl;

        std::cout << "> ";
        std::string client_message{};
        std::getline(std::cin, client_message);
        send(client_socket, client_message.c_str(), sizeof(client_message), 0);
      }
    }
  }
}
