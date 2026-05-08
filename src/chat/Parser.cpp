#include "Parser.h"
#include "Message.h"

#include <iostream>

chat::Message chat::Parser::ParseBytes(std::string& bytes){
  Message msg{};
  std::cout << "Parsing: " << bytes << std::endl;

  msg.m_prefix = chat::Parser::GetPrefix();
  return msg;
}

std::string chat::Parser::GetPrefix(){
  std::string prefix{};
  return prefix;
}

std::vector<std::string> chat::Parser::GetArguments(){
  std::vector<std::string> args{};
  return args;
}

std::string chat::Parser::GetTrailing(){
  std::string trailing{};
  return trailing;

}
