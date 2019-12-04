CC = g++
CXXFLAGS = -g -c -std=c++11
OBJS = Ngrams.o WordList.o main.o
LFLAGS = -pg

.C.o:
	$(CC) $(CXXFLAGS) $< -o $@

my-ngram: WordList.o StringNode.o HashTable.o LinkedList.o NgramListCustom.o
	$(CC) $(CXXFLAGS) -DCUSTOM main.C -o main.o
	$(CC) $(LFLAGS) WordList.o StringNode.o HashTable.o LinkedList.o NgramListCustom.o main.o -o my-ngram

ngramslow: WordList.o NgramListOriginal.o
	$(CC) $(CXXFLAGS) -DORIGINAL main.C -o main.o
	$(CC) $(LFLAGS) WordList.o NgramListOriginal.o main.o -o ngramslow

ngram: WordList.o NgramListSTL.o
	$(CC) $(CXXFLAGS) -DSTL main.C -o main.o
	$(CC) $(LFLAGS) WordList.o NgramListSTL.o main.o -o ngram

WordList.o: WordList.h

StringNode.o: StringNode.h

HashTable.o: HashTable.h StringNode.h

LinkedList.o: StringNode.h

NgramListCustom.o: NgramListCustom.h WordList.h

NgramListOriginal.o: NgramListOriginal.h WordList.h

NgramListSTL.o: NgramListSTL.h WordList.h

clean:
	-rm -f *.o ngramfast ngramslow ngramstl gmon.out 
