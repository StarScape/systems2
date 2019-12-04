#include <iostream>
#include "LinkedList.h"

LinkedList::LinkedList() {
}

void LinkedList::deleteList(StringNode **head) {
  /* deref head to get the real head */
  StringNode* current = *head;  
  StringNode* next;  
    
  while (current != NULL)  
  {  
      next = current->next;  
      delete current;  
      current = next;  
  }  
        
  /* deref head to affect the real head back  
      in the caller. */
  *head = NULL;
}


LinkedList::~LinkedList() {
  deleteList(&first);
}

void LinkedList::insertEnd(std::string s, int val) {
  if (first == NULL)
  {
    first = new StringNode(s, val, NULL);
    last = first;
  }
  else {
    last->next = new StringNode(s, val, NULL);
    last = last->next;
  }
}