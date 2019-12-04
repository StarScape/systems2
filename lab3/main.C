/*
 * Name: Jack A. Arrington
 *
 * Name:
 *
 */


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "WordList.h"

#ifdef ORIGINAL
   #include "NgramListOriginal.h"
#endif

#ifdef CUSTOM
   #include "NgramListCustom.h"
#endif

#ifdef STL
   #include "NgramListSTL.h"
#endif

// #include "NgramList.h"

using namespace std;

/*
 * usage: ngram [-n ngramsize] filename
 *
 * opens the text file provided as input and builds
 * ngrams out of the words in the file. An ngram is 
 * a sequence of words that are not separated by
 * punctuation such as , . ! ? ; :
 * The size of the ngrams to be built is provided by
 * the optional ngramsize parameter; the default is 2.
 * The program also keeps a count of unique ngrams
 * and sorts the ngrams by count. At the end, the
 * ngrams and their counts are output to standard out.
 */

void usage(int argc, char * argv[], int & ngrams, string & filename);

/*
 * main
 *
 * drives the ngram building process
 *
 * param: int argc - number of command line arguments (either 2 or 4)
 * param: char * argv[] - command line arguments
 *        argv[0] - name of executable
 *        argv[1] - either input file name or optional "-n"
 *        argv[2] - either ngramsize or not used
 *        argv[3] - either input file name or not used
 */
int main(int argc, char * argv[])
{
   string filename;
   string word;
   int ngrams;
   WordList wl;

   usage(argc, argv, ngrams, filename);

   ifstream wordfile(filename);
   while (wordfile >> word)
   {
      wl.addWord(word);
   }

   NgramList nglst(ngrams, wl);
   nglst.sortByCount();
   cout << nglst;
}

/*
 * usage
 *
 * parses the command line arguments 
 *
 * param: int argc - number of command line arguments
 * param: char * argv[] - command line arguments
 * param: int & ngrams - set to the command line arguments (default 2)
 * param: string & filename - set to the name of the text file to be 
 *                            used for input
 */
void usage(int argc, char * argv[], int & ngrams, string & filename)
{
   if (((argc != 2) && (argc != 4)) || 
       ((argc == 4) && (strcmp(argv[1], "-n") != 0)))
   {
      cout << "usage: " << argv[0] << " [-n <ngrams> ] <filename>\n";
      exit(1);
   } else if (argc == 2)
   {
      ngrams = 2; 
      filename = argv[1];
   } else 
   {
      ngrams = atoi(argv[2]); 
      filename = argv[3];
   }
   ifstream wordfile(filename);
   if (!wordfile.is_open()) 
   {
      cout << "could not open: " << filename << endl;
      cout << "usage: " << argv[0] << " [-n <ngrams> ] <filename>\n";
      exit(1);
   }
   wordfile.close();
}
