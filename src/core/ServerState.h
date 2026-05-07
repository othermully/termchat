#ifndef SERVERSTATE_H
#define SERVERSTATE_H

#include <unordered_map>
#include <vector>
#include <string>

#include "../net/Client.h"

namespace core{

class Channel;
struct Client;

class ServerState{
public:

  void JoinChannel();
  void LeaveChannel();
  void AuthClient();

  void AddConnectedClient();

private:
  // fd -> Client
  std::unordered_map<int, Client>          connected_clients{};

  // Chan name -> Channel
  std::unordered_map<std::string, Channel> channels{};

};
} // core

#endif
