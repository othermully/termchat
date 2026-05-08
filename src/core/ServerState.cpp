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

  core::Client client{ fd };
  client.state = core::ClientState::AUTH;
  connected_clients.insert( {fd, client} );

  std::cout << "Client " << fd << " authenticated, need to register.\n";

  return;
}
