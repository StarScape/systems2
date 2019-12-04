#include <iostream>
#include "HashTable.h"

int HashTable::hash(std::string word) {
   int seed = 176; 
   unsigned long hash = 0;

   for(int i = 0; i < word.length(); i++) {
      hash = (hash * seed) + word[i];
   }
   return hash % capacity;
}

HashTable::HashTable(int cap) {
  capacity = cap;
  arr = new StringNode*[capacity];
}

HashTable::~HashTable() {
  for (int i = 0; i < capacity; ++i)
  {
    delete arr[i];
  }
  delete[] arr;
}

void HashTable::insert(std::string s, int val) {
  int i = hash(s);
  // std::cout << "inserting: " << s << std::endl;
  StringNode *start = arr[i];

  if (start == NULL) {
    arr[i] = new StringNode(s, val, NULL);
  }
  else {
    while (start->next != NULL) {
      start = start->next;
    }
    start->next = new StringNode(s, val, NULL);
  }
}

StringNode* HashTable::find(std::string s) {
  int i = hash(s);
  StringNode *start = arr[i];

  if (start != NULL && start->key == s) {
    return start;
  }

  while (start != NULL && start->key != s) {
    start = start->next;
  }

  return start;
}
