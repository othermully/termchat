#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

namespace chat{

struct Message;

class Parser{
public:

  static Message ParseBytes(std::string& bytes);

  
private:
  std::string 		   GetPrefix();
  std::vector<std::string> GetArguments();
  std::string 		   GetTrailing();

  std::string m_raw_bytes_;

};
} // chat
#endif
