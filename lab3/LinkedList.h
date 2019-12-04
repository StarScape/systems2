#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <string>
#include "StringNode.h"

class LinkedList {
   public:
      StringNode *first;
      StringNode *last;

      LinkedList();
      ~LinkedList();
      void insertEnd(std::string s, int val);
      void deleteList(StringNode **head);
};



#endif