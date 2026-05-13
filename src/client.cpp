#include <cerrno>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <fcntl.h>


int setNonBlocking(int fd){
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK); 
}

int main(){

  std::vector<pollfd> pfds{};

  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  setNonBlocking(client_socket);

  struct sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9001);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  socklen_t len = sizeof(server_addr);

  if (connect(client_socket, (sockaddr*)&server_addr, len) < 0) {
    if (errno != EINPROGRESS) {
      perror("connect failure: ");
      exit(1);
    }
  }

  std::cout << "Connected to server.\n";

  pollfd client_poll;
  client_poll.fd = client_socket;
  client_poll.events = POLLIN;
  pfds.push_back(client_poll);

  setNonBlocking(client_poll.fd);

  while (true) {
    char buffer[1024] = { '\0' };
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::cout << buffer << std::endl;

    std::cout << "> ";
    std::string client_msg{};
    std::getline(std::cin, client_msg);
    send(client_socket, client_msg.c_str(), sizeof(client_msg), 0);
  }

  return 0;
}
