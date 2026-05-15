#include <cerrno>
#include <cstdio>
#include <ostream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <unistd.h>

int SetNonBlocking(int fd){
  int flags = fcntl(fd, F_GETFL, 0); 
  if (flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK); 
}

int main(){

  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  SetNonBlocking(client_socket);



  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9001);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  socklen_t len = sizeof(server_addr);

  if(connect(client_socket, (sockaddr*)&server_addr, len) < 0){
    if (errno != EINPROGRESS) {
      perror("Connect failure: ");
      exit(1);
    }
  }

  pollfd pfds[2];
  pfds[0].fd = STDIN_FILENO;
  pfds[0].events = POLLIN;
  pfds[1].fd = client_socket;
  pfds[1].events = POLLIN;


  while (true) {

    std::cout << "> " << std::flush;

    int ret = poll(pfds, 2, -1);
    if (ret < 0){
      perror("poll");
      break;
    }

    //stdin ready
    if (pfds[0].revents & POLLIN) {
      std::string msg;
      std::getline(std::cin, msg);
      send(client_socket, msg.c_str(), msg.size(), 0);
    }

    // socket ready
    if (pfds[1].revents & POLLIN) {
      char buffer[1024];

      ssize_t bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);

      if (bytes > 0) {
        buffer[bytes] = '\0';
        std::cout << "Server: " << buffer << std::endl;
      }
    }
  }

  return 0;
}
