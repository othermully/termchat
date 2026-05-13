#include <iostream>

#include "Parser.h"
#include "Message.h"


chat::Message chat::Parser::ParseBuffer(std::string& buffer){
  Message msg{};

  if (buffer[0] == '!') {
    std::string cmd = chat::Parser::ParseCommand(buffer);
    msg.command = cmd;
  }

  else {
    msg.chat_message = buffer;
  }

  return msg;
}

std::string chat::Parser::ParseCommand(const std::string& str){
  std::string command{};

  size_t pos = str.find(' ');
  command = str.substr(1, pos);

  std::cout << "Got command: " << command << std::endl;

  return command;
};

