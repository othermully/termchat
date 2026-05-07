#include "Server.h"

#include <cerrno>
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#include "../core/ServerState.h"
#include "../net/Client.h"

#define BUFFER_SIZE 4096

// constructor
net::Server::Server(const uint16_t port, std::string& name){
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    throw std::runtime_error("socket error");
    exit(1);
  }

  struct sockaddr_in     addr{};
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_socket, (sockaddr*)&addr, sizeof(addr)) < 0) {
    throw std::runtime_error("bind error");
    exit(1);
  }

  if (listen(server_socket, SOMAXCONN) < 0) {
    throw std::runtime_error("listen error");
    exit(1);
  }

  // add listening socket to file descriptors to poll 
  struct pollfd pfd{};
  pfd.fd 	= server_socket;
  pfd.events 	= POLLIN | POLLOUT;
  pollfds.push_back(pfd);

  // assign listening fd to servers fd
  m_fd   = server_socket;
  m_name = name;

  std::cout << m_name << " started... ";
  std::cout << "Listening on port " << port << std::endl;
}

// accept client, create client object, set client state, add to connected_clients
// only add to polling pfds once client is fully registered
net::Client net::Server::AcceptClient(){
  int client_fd = accept(Server::m_fd, nullptr, nullptr);
  if (client_fd < 0) {
    std::cout << "Failed to accept client: " << errno << std::endl;
    errno = 0;
  }

  net::Client new_client { client_fd };
  new_client.m_fd = client_fd;
  new_client.state = net::ClientState::AUTH;

  return new_client;
}

// Main event loop
void net::Server::Start(){
  // Initialize server state 
  core::ServerState state;

  while (true) {
  
  }
};
