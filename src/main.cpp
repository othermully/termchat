#include "./net/Server.h"
#include <cstdint>

int main(){
  constexpr uint16_t port { 9001 };

  std::string name { "My server" };
  net::Server server(port, name);
  return 0;
}
