// Jack Arrington
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Data types 
typedef struct {
  int S;
  int B;
  int E;
  int s;
  int b;
  int hits;
  int misses;
  int evictions;
  char* trace;
} CacheParams;

typedef struct {
  int valid;
  char *block;
  unsigned long tag;
  int usedCounter;
} Line;

typedef struct {
  Line *lines;
} Set;

typedef struct {
  Set *sets;
} Cache;

Cache initCache(long long S, int E) {
  Cache cache;  
  Set set;
  Line line;
  cache.sets = (Set *) malloc(sizeof(Set) * S);

  // Allocate dem dataz
  for (int i = 0; i < S; i++) {
    set.lines =  (Line *) malloc(sizeof(Line) * E);
    cache.sets[i] = set;

    for (int j = 0; j < E; j++) {
      line.valid = line.tag = line.usedCounter = 0;
      set.lines[j] = line;
    }
  } 
  return cache;
}

// Returns true if hit
bool isHit(Line line, unsigned long tag) {
  if (line.valid && line.tag == tag) {
    return true;
  }
  return false;
}

// Returns true if set is full
bool isFull(Set set, CacheParams params) {
  for (int i = 0; i < params.E; i++) {
    if (!set.lines[i].valid) {
      return true;
    }
  }
  return false;
}

// Returns index of next open line
int findOpen(Set set, CacheParams params) {
  for (int i = 0; i < params.E; i++) {
    if (!set.lines[i].valid) {
      return i;
    }
  }

  // This should never, ever happen...
  return -1;
}

// Returns index of line to evict
int findToEvict(Set set, CacheParams params) {
  int min = set.lines[0].usedCounter;
  int indexToEvict = 0;

  for (int i = 0; i < params.E ; i++) {
    if (min > set.lines[i].usedCounter) {
      indexToEvict = i;
      min = set.lines[i].usedCounter;
    }
  }

  return indexToEvict;
}

int findMax(Set set, CacheParams params) {
  int max = set.lines[0].usedCounter;
  int maxIndex = 0;

  for (int i = 0; i < params.E ; i++) {
    if (set.lines[i].usedCounter > max) {
      maxIndex = i;
      max = set.lines[i].usedCounter;
    }
  }

  return maxIndex;
}

CacheParams testCache(Cache cache, CacheParams params, unsigned long address) {
  // Compute the size of the tag, 64 bit system
  int tagSize = 64 - (params.b + params.s);
  unsigned long tag = address >> (params.s + params.b);

  // Use the tagSize to compute for the set index
  unsigned long temp = address << (tagSize);
  unsigned long setIndex = temp >> (tagSize + params.b);
  Set set = cache.sets[setIndex];

  bool hit = false;

  for (int i = 0; i < params.E; i++) {
      Line currentLine = set.lines[i];

      if (isHit(currentLine, tag)) {
        params.hits++;
        int max = findMax(set, params);
        cache.sets[setIndex].lines[i].usedCounter = cache.sets[setIndex].lines[max].usedCounter + 1;
        hit = true;
      }
  }

  if (!hit && isFull(set, params)) {
      params.misses++;
      int index = findOpen(set, params);

      set.lines[index].tag = tag;
      set.lines[index].valid = 1;

      int max = findMax(set, params);
      cache.sets[setIndex].lines[index].usedCounter = cache.sets[setIndex].lines[max].usedCounter + 1;
  }

  else if (!hit) {
    params.misses++;
    params.evictions++;

    int evictIndex = findToEvict(set, params);
    set.lines[evictIndex].tag = tag;

    int max = findMax(set, params);
    cache.sets[setIndex].lines[evictIndex].usedCounter = cache.sets[setIndex].lines[max].usedCounter + 1;
  }

  return params;
}

CacheParams initParamsFromArgs(int argc, char** argv) {
  CacheParams params;
  char input = getopt(argc, argv, "s:E:b:t:vh");

  while(input != -1)
  {
    switch(input) {
    case 's':
      params.s = atoi(optarg);
      break;
    case 'E':
      params.E = atoi(optarg);
      break;
    case 'b':
      params.b = atoi(optarg);
      break;
    case 't':
      params.trace = optarg;
      break;
    case 'v':
      break;
    case 'h':
      usage();
      exit(0);
    default:
      usage();
      exit(-1);
    }

    input = getopt(argc, argv, "s:E:b:t:vh");
  }

  params.S = pow(2.0, params.s);
  params.B = pow(2.0, params.b);
  params.misses = params.hits = params.evictions = 0;

  return params;
}

void runTrace(CacheParams* params, Cache* cache) {
  char command;
  unsigned long address;
  int size;

  FILE *file = fopen(params->trace, "r");
  
  while(fscanf(file, " %c %lx,%d", &command, &address, &size) == 3) {
    switch(command) {
      case 'L':
        *params = testCache(*cache, *params, address);
        break;
      case 'S':
        *params = testCache(*cache, *params, address);
        break;
      case 'M':
        *params = testCache(*cache, *params, address);
        *params = testCache(*cache, *params, address);  
        break;
    }
  }

  fclose(file);
}

void usage() {
  printf("Usage: ./csim [-h] [-v] -s <s> -E <E> -b <b> -t <tracefile>\n");
  printf("-s: number of set index(2^s sets)\n");
  printf("-E: number of lines per set\n");
  printf("-b: number of block offset bits\n");
  printf("-t: trace file name\n");
}

int main(int argc, char** argv) {
  CacheParams params = initParamsFromArgs(argc, argv);
  Cache cache = initCache(params.S, params.E);
  runTrace(&params, &cache);
  printSummary(params.hits, params.misses, params.evictions);
  return 0;
}
