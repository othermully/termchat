// TODO: Server::HandleRead, ServerState::CommandDispatcher, Server::SendToClient, ServerState::RegisterClient

#include "Server.h"

#include <algorithm>
#include <cerrno>
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

#include "../core/ServerState.h"
#include "../chat/Parser.h"
#include "../chat/Message.h"

#define BUFFER_SIZE 4096


int net::setNonBlocking(int fd){
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK); 
}

void net::Server::CleanupFd(int fd){
  close(fd);
  pollfds.erase(
      std::remove_if(pollfds.begin(), pollfds.end(), [&](pollfd const & pfd){
	return pfd.fd == fd;
	}),
      pollfds.end()
      );
}

// constructor
net::Server::Server(const uint16_t port, std::string& name){
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    throw std::runtime_error("socket error");
    exit(1);
  }

  setNonBlocking(server_socket);

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

void net::Server::HandleRead([[maybe_unused]] core::ServerState& state, int fd){
  // TODO: throw bytes into parser, generate message, dispatch based on command
  char buffer[BUFFER_SIZE];

  ssize_t bytes_received = recv(fd, buffer, sizeof(buffer), 0);
  if (bytes_received > 0) 
  {

    std::string msg(buffer);
    std::cout << "Received: " << msg << std::endl;

    if (!state.CheckAuthenticated(fd)) {
      state.AuthClient(fd, msg);
      std::string response { "200 AUTH OK\n" };

      send(fd, response.c_str(), response.size(), 0);
    }
    else {
      std::string response = "Echo: " + msg;

      send(fd, response.c_str(), response.size(), 0);
    }

    return;
  }

  else if (bytes_received == 0) {
    // This is how disconnects should be handled, i think
    std::cout << "Lost client connection.\n";
    state.DisconnectClient(fd); // Remove from connected_clients;

    // Close file descriptor, remove from polligns fds
    CleanupFd(fd);
    
    return;
  }

  else {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // nothing to read right now
      return;
    }

    // actual socket error here
    state.DisconnectClient(fd);
    CleanupFd(fd);

    return;
  }
}

int net::Server::AcceptClient(core::ServerState& state){
  int client_fd = accept(Server::m_fd, nullptr, nullptr);
  if (client_fd < 0) {
    std::cout << "Failed to accept client: " << errno << std::endl;
    errno = 0;
  }

  setNonBlocking(client_fd);

  pollfd pfd{};
  pfd.fd     = client_fd;
  pfd.events = POLLIN;
  pollfds.push_back(pfd);

  core::Client client;
  client.m_fd = client_fd;
  client.state = core::ClientState::CONNECTED;
  state.AddConnectedClient(client);

  std::cout << "Client " << client_fd << " accepted.\n";


  return client_fd;
}

void net::Server::SendToClient(int fd, core::ServerState& state, [[maybe_unused]]std::string& msg){
      send(fd, state.GetClient(fd).m_output_buffer.c_str(), state.GetClient(fd).m_output_buffer.size(), 0);
}

// Main event loop
void net::Server::Start(){
  // Initialize server state 
  core::ServerState state{};

  while (true) {
    int ret = poll(pollfds.data(), pollfds.size(), -1);
    if (ret <= 0) continue;

    for (size_t i = 0; i < pollfds.size(); ++i) {
      // Read to read
      if (pollfds[i].revents & POLLIN) {
	// New connection
        if (pollfds[i].fd == m_fd) {
	  [[maybe_unused]] int client_fd = AcceptClient(state);

        }
	// Existing connection
        else {
	  HandleRead(state, pollfds[i].fd);
        }
      }
    }
  }
};
