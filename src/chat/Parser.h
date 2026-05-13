#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

namespace chat{

struct Message;

class Parser{
public:

  static Message ParseBuffer(std::string& buffer);
  
private:
  static std::string ParseCommand(const std::string& str);

};
} // chat
#endif
