class NgramList 
{
   struct Ngram
   {
      std::string ngram;
      int count;
      struct Ngram * next;
   };

   typedef struct Ngram Ngram_t;

   private:
      Ngram_t * first;
      int ngramSz;
      void insertNgram(std::string s);
      std::string getNextNgram(WordList::const_iterator start, WordList::const_iterator end);
      int size, maxCount;
      Ngram_t** array;

   public:
      NgramList(int ngramSz, const WordList & wl);
      ~NgramList();
      void sortByCount();
      friend std::ostream& operator<<(std::ostream& os, const NgramList & nl);
};
