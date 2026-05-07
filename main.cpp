#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <unordered_map>
#include <vector>

#define BUFFER_SIZE 4096

int setNonBlocking(int fd){
  int flags = fcntl(fd, F_GETFL, 0); 
  if (flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK); 
}

namespace CHANNEL{
class Channel{
public:

  void Broadcast(const std::string& message){
    std::string msg = m_name_ + ": " + message;
    for (size_t i = 0; i < m_members_.size(); ++i) {
      send(m_members_[i], msg.c_str(), sizeof(msg), 0);
    }
  }

  bool CheckMember(const int fd){
    if (std::find(m_members_.begin(), m_members_.end(), fd) != m_members_.end()) {
      return true;
    }

    return false;
  }

  void AddMember(int fd){
    std::string msg { "You were added to the " + m_name_ + " channel" };
    m_members_.push_back(fd);
    send(fd, msg.c_str(), sizeof(msg), 0);

    std::cout << "Client fd: " << fd << " added to chan: " << m_name_ << std::endl;

    return;
  }

  std::string GetName(){
    return m_name_;
  }

  std::vector<int>& getChanMembers(){
    return m_members_;
  }

  Channel(std::string chan_name, int chan_id)
    : m_name_ { chan_name }, m_channel_id_ { chan_id } 
  {
    std::cout << "Channel " << chan_name << "(id: " << chan_id << ")" << " created.\n";
  }

private:
  std::string      m_name_{};
  int              m_channel_id_{};
  std::vector<int> m_members_{}; // Client fds
};

} // CHANNEL

namespace CLIENT{
struct Client{
  int fd_{};
  std::string nickname{};
};
} // CLIENT

namespace STATE{
class ServerState{
public:

  void JoinChannel(int fd, CHANNEL::Channel& chan){
    chan.AddMember(fd);

    std::string msg = "You have joined the " + chan.GetName() + " channel.\n";

    send(fd, msg.c_str(), sizeof(msg), 0);
    std::cout << connected_clients[fd].nickname << " has joined the " << chan.GetName() << " channel." << std::endl;

    return;
  };

  void LeaveChannel(int fd, CHANNEL::Channel& chan);

  void DisconnectClient(const int fd){
    close(fd);
    connected_clients.erase(fd);
    std::cout << "Successfully disconnected client " << fd << std::endl;
    return;
  };

  // Command dispatcher
  void HandleClient(const int fd){
    //TODO: Check if client is in a channel.
    // FD is the client within CHANNEL.members_

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(fd, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
      // Handle bytes
      std::cout << connected_clients[fd].nickname << " :> " << buffer << std::endl;
    }

    else if (bytes_received == 0) {
      std::cout << "Lost client " << "(" << connected_clients[fd].nickname << ")" << " connection.\n";
      DisconnectClient(fd);
      return;
    }

    else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // nothing to read right now
        return;
      }

      // actual socket error
      DisconnectClient(fd);
      return;
    }

    return;
  }

  void AuthClient(const int fd){
    // TODO: Accept client -> Transistion to auth state -> later POLLIN arrives -> Read nick incrementally -> transition to CONNECTED state

    std::string welcome_msg = { "Welcome, please enter a nickname:" };
    send(fd, welcome_msg.c_str(), sizeof(welcome_msg), 0);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(fd, buffer, sizeof(buffer), 0);
    if (bytes_received == 0) {
      std::cout << "Lost client " << fd << " connection.\n";
      DisconnectClient(fd);

      return;
    }

    else if (bytes_received > 0) {
      CLIENT::Client new_client{};
      new_client.fd_      = fd;
      new_client.nickname = buffer;

      connected_clients.insert( { new_client.fd_, new_client } );
      std::cout << "Client " << "'" << new_client.nickname << "'" << " accepted.\n";
    }

    else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        //  nothing to read right now
        return;
      }

      // Actual socket error
      DisconnectClient(fd);
      return;
    }

    return;
  };

  void AddChannel(int id, CHANNEL::Channel& chan){
    channels.insert( {id, chan });
  };

private:

  std::unordered_map<int, CLIENT::Client>   connected_clients{};
  std::unordered_map<int, CHANNEL::Channel> channels{};

};
} // STATE

namespace SERVER{
class Server{
public:

  int fd{};

  // Main event loop
  void Run(){
    STATE::ServerState server_state;

    CHANNEL::Channel main_chan( { "#main", 0} );
    server_state.AddChannel(0, main_chan);

    int timeout_msec { 500 };

    while (true) {
      int ret = poll(pfds.data(), pfds.size(), timeout_msec);
      if (ret <= 0) continue;

      for (size_t i = 0; i < pfds.size(); ++i) {
        if (pfds[i].revents & POLLIN) {

          if (pfds[i].fd == fd) {
            // New connection
            int client_conn = accept(fd, nullptr, nullptr);

            setNonBlocking(client_conn);

            pollfd client_poll;
            client_poll.fd = client_conn;
            client_poll.events = POLLIN;
            pfds.push_back(client_poll);

            server_state.AuthClient(client_conn);
            //server_state.JoinChannel(client_conn, main_chan);
            //main_chan.AddMember(client_conn);

          }
          else {
            // Existing connection
            server_state.HandleClient(pfds[i].fd);
            //main_chan.Broadcast("Hello from main_chan!");
          }

        }
      }
    }
  };

  Server(const uint16_t port){
    // create listening socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
      throw std::runtime_error("socket error");
      exit(1);
    }

    fd = server_socket;
    setNonBlocking(server_socket);

    // define addr stuct
    sockaddr_in addr{};
    addr.sin_family       = AF_INET;
    addr.sin_port         = htons(port);
    addr.sin_addr.s_addr  = INADDR_ANY;
    socklen_t len         = sizeof(addr);

    // bind socket
    if (bind(fd, (sockaddr*)&addr, len) < 0) {
      throw std::runtime_error("bind error");
      exit(1);
    }

    // listen on socket
    if (listen(fd, SOMAXCONN) < 0) {
      throw std::runtime_error("listen error");
      exit(1);
    }

    struct pollfd pfd{};
    pfd.fd      = server_socket;
    pfd.events  = POLLIN;
    pfds.push_back(pfd);

    std::cout << "Listening on port " << port << std::endl;
  }

private:

  std::vector<pollfd> pfds{};
};
} // SERVER
  
namespace PARSER {
class Parser{
public:
  void ParseMessage(){
  };

private:
  std::string m_msg_{};
};
} // PARSER


int main(){
  constexpr uint16_t port { 9001 };
  SERVER::Server server(port);
  server.Run();
  return 0;

}
