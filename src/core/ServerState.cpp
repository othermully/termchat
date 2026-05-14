#include "ServerState.h"
#include "Client.h"

#include <iostream>
#include <unistd.h>

void core::ServerState::AddConnectedClient(core::Client& client){
  connected_clients.insert({client.m_fd, client});
}

void core::ServerState::DisconnectClient(int fd){
  close(fd);
  connected_clients.erase(fd);
}

bool core::ServerState::CheckAuthenticated(int fd){
  if (ServerState::GetClient(fd).state == ClientState::REGISTERED) {
    return true;
  }
  return false;
}

void core::ServerState::AuthClient(int fd, std::string& msg){

  auto& client = connected_clients.at(fd);
  client.m_nickname = msg;
  client.state = ClientState::REGISTERED;

  std::cout << "client: " << client.m_nickname << " Registered" << std::endl;
  return;
}

