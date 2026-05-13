#include "ServerState.h"
#include "Client.h"

#include <iostream>
#include <unistd.h>

void core::ServerState::DisconnectClient(int fd){
  close(fd);
  connected_clients.erase(fd);
}


void core::ServerState::AuthClient(int fd){
  std::cout << "Authenticating client: " << fd << std::endl;

  core::Client client;
  client.m_fd = fd;
  client.state = core::ClientState::AUTH;
  connected_clients.insert( {fd, client} );

  std::cout << "Client " << fd << " authenticated, need to register.\n";

  return;
}

void core::ServerState::CommandDispatcher(std::string& cmd, core::Client& client){

  if (cmd == "REGISTER") {
    RegisterClient(client);
  }

  else {
    std::cout << "Command not found.\n";
  }

}


void core::ServerState::RegisterClient([[maybe_unused]] core::Client& client){
  std::cout << "Client registration...\n";
  client.m_output_buffer = "Enter a nickname: ";
}

