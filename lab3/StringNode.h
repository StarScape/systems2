#ifndef SRINGNODE_H_
#define SRINGNODE_H_

#include <string>

class StringNode {
   public:
      StringNode *next;
      std::string key;
      int val;

      StringNode();
      StringNode(std::string s, int v, StringNode *next);
      ~StringNode();
};



#endif