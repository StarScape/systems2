#ifndef NGRAM_H_
#define NGRAM_H_

#include "HashTable.h"
#include "StringNode.h"
#include "LinkedList.h"

using namespace std;

class NgramList 
{
   private:
      int ngramSz;
      int numNgrams;
      std::string getNextNgram(WordList::const_iterator start, 
                               WordList::const_iterator end);
      void insertNgram(std::string s);
      HashTable *ngrams;
      LinkedList *ngramsSorted;
   public:
      NgramList(int ngramSz, const WordList & wl);
      ~NgramList();
      void sortByCount();      
      friend std::ostream& operator<<(std::ostream& os, const NgramList & nl);
};



#endif