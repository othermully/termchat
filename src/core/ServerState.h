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

  void AuthClient(int fd, std::string& msg);
  void DisconnectClient(int fd);

  bool CheckAuthenticated(int fd);

  void AddConnectedClient(Client& client);

  void RegisterClient(core::Client& client);

  void CommandDispatcher(std::string& cmd, core::Client& client);

  core::Client& GetClient(int fd){
    return connected_clients.at(fd);
  }


private:
  // fd -> Client
  std::unordered_map<int, core::Client>          connected_clients{};

  // Chan name -> Channel
  //std::unordered_map<std::string, Channel> channels{};

};
} // core

#endif
