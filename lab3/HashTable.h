#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <string>
#include "StringNode.h"

class HashTable {
   private:
      int capacity;

      int hash(std::string s);

   public:
      StringNode **arr;
      HashTable(int capacity);
      ~HashTable();
      void insert(std::string s, int val);
      StringNode* find(std::string);
};



#endif