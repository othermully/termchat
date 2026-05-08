#ifndef SERVERSTATE_H
#define SERVERSTATE_H

#include <unordered_map>
#include <vector>
#include <string>


#include "../core/Client.h"


namespace core{
class ServerState{
public:

  void JoinChannel();
  void LeaveChannel();

  void AuthClient(int fd);
  void DisconnectClient(int fd);

  void AddConnectedClient();

  core::Client& GetClient(int fd){
    return connected_clients[fd];
  }


private:
  // fd -> Client
  std::unordered_map<int, core::Client>          connected_clients{};

  // Chan name -> Channel
  //std::unordered_map<std::string, Channel> channels{};

};
} // core

#endif
