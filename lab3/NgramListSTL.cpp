#include <iostream>
#include <string>
#include <vector>
#include "WordList.h"
#include "NgramListSTL.h"


using namespace std;

/*
 * NgramList
 *
 * takes as input the size of the ngrams to be built and the list  
 * of words to build the ngrams from and builds a linked list of 
 * ngrams.
 *
 * param: int ngramSz - size of the ngram
 * param: const WordList & wl - list of the words use
 */
NgramList::NgramList(int ngramSz, const WordList & wl)
{
    this->ngramSz = ngramSz;
    WordList::const_iterator p;
    p = wl.begin();
    while (p != wl.end())
    {
        std::string ngram = getNextNgram(p, wl.end());
        p++;
        if (!ngram.empty()) insertNgram(ngram);
    }
    sortByCount();
}


/*
 * NgramList destructor
 *
 * automatically called when NgramList object goes out of scope
 * should delete any space dynamically allocated for the ngram
 */
NgramList::~NgramList()
{
}

/*
 * getNextNgram
 *
 * takes as input an iterator that points to the word to start the
 * ngram and an iterator that points to the end of the word list
 * and builds and returns the ngram. returns empty string if no
 * ngram can be built, for example, if there are insufficient words
 * to build one.
 *
 * param: WordList::const_iterator start - points to first word in ngram
 * param: WordList::const_iterator end - points to end of word list
 * return: std::string - returns ngram or empty string
 */
std::string NgramList::getNextNgram(WordList::const_iterator start, 
        WordList::const_iterator end)
{
    int i, len;
    std::string ngram = "";
    for (i = 0; i < ngramSz && start != end; i++)
    {
        std::string word = (*start);
        //see if the string ends with punctuation
        //don't create ngrams that continue after punctuation
        if (!isalpha(word[word.length()-1]) && i < ngramSz - 1) return "";

        //take off all ending punctuation
        len = word.length() - 1;
        while (len >= 0 && !isalpha(word[len])) 
        {
            //remove last character
            word = word.substr(0, word.length() - 1);
            len--;
        }
        if (len < 0) return "";  //give up

        //is the first word in the ngram?
        if (ngram == "") ngram = word;
        else ngram = ngram + " " + word;

        start++;
    }

    //could we build a long enough ngram?
    if (i < ngramSz) return "";

    //take off beginning punctuation
    while (ngram.length() > 0 && !isalpha(ngram[0])) 
        ngram = ngram.substr(1, ngram.length());
    return ngram;
}


/*
 * insertNgram
 *
 * Inserts ngram into whatever structure you choose to hold
 * your ngrams.
 *
 * param: std::string s - ngram to be inserted
 * return: none
 */
void NgramList::insertNgram(std::string s)
{
    auto contains = ngrams.find(s);

    if (contains != ngrams.end()) {
        ++(contains->second);
    }
    else {
        ngrams.insert({s, 1});    
    }
}

void NgramList::sortByCount()
{
    numNgrams = ngrams.size();
    ngramsSorted = new std::vector<std::string>[numNgrams];

    for (auto ngram : ngrams)
    {
        std::string s = ngram.first;
        int occurences = ngram.second;
        ngramsSorted[occurences].push_back(s);
    }
}

std::ostream& operator<<(std::ostream& os, const NgramList & nglst)
{
    std::cout << "List of " << nglst.ngramSz << " word ngrams and counts\n";
    std::cout << "--------------------------------\n";
    for (int i = nglst.numNgrams; i >= 0; i--)
    {
        if (nglst.ngramsSorted[i].size() > 0) {
            // std::cout << nglst.ngramsSorted[i].size() << std::endl;
            for (std::string ngram : nglst.ngramsSorted[i]) {
                std::cout << ngram << ", " << i << endl;
            }
        }
    }
    return os;
}