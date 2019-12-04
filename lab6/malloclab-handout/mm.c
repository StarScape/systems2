/*
 * REPLACE this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

team_t team = {
    /* Team name */
    "Team Awesome",
    /* First member's full name */
    "Jack Arrington",
    /* First member's email address */
    "arringtonja1@appstate.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// Define a whole heap o' macros
#define WSIZE      sizeof(void *)
#define DSIZE      (2 * WSIZE)
#define CHUNKSIZE  (1 << 12)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PACK(size, alloc)  ((size) | (alloc))
#define GET_PTR(p)       (*(uintptr_t *)(p))
#define PUT_PTR(p, val)  (*(uintptr_t *)(p) = (val))
#define GET_SIZE(p)   (GET_PTR(p) & ~(DSIZE - 1))
#define GET_ALLOC(p)  (GET_PTR(p) & 0x1)
#define HEADER(block_ptr)  ((void *)(block_ptr) - WSIZE)
#define FOOTER(block_ptr)  ((void *)(block_ptr) + GET_SIZE(HEADER(block_ptr)) - DSIZE)
#define NEXT_BLOCK(block_ptr)  ((void *)(block_ptr) + GET_SIZE(HEADER(block_ptr)))
#define PREV_BLOCK(block_ptr)  ((void *)(block_ptr) - GET_SIZE((void *)(block_ptr) - DSIZE))
#define GET_NEXT_PTR(block_ptr)  (*(char **)(block_ptr + WSIZE))
#define GET_PREV_PTR(block_ptr)  (*(char **)(block_ptr))
#define SET_NEXT_PTR(block_ptr, p) (GET_NEXT_PTR(block_ptr) = p)
#define SET_PREV_PTR(block_ptr, p) (GET_PREV_PTR(block_ptr) = p)

// Funtion protos
static void *expandHeap(size_t words);
static void *coalesce(void *block_ptr);
static void *fitBlock(size_t size_adjusted);
static void place(void *block_ptr, size_t size_adjusted);

static void insertList(void *block_ptr); 
static void removeList(void *block_ptr); 

// The only global var we need :)
static char *free_list = 0;

// Util functions below

// Boundary tag coalescing 
// Returns addr of coalesced block
static void *coalesce(void *block_ptr) {
  // If prev block already allocated or its size is zero then PREV_ALLOC will be set.
  size_t NEXT_ALLOC = GET_ALLOC(HEADER(NEXT_BLOCK(block_ptr)));
  size_t PREV_ALLOC = GET_ALLOC(FOOTER(PREV_BLOCK(block_ptr))) || PREV_BLOCK(block_ptr) == block_ptr;
  size_t size = GET_SIZE(HEADER(block_ptr));
  
  /// Next block is only free
  if (PREV_ALLOC && !NEXT_ALLOC) {                  
    size += GET_SIZE( HEADER(NEXT_BLOCK(block_ptr))  );
    removeList(NEXT_BLOCK(block_ptr));
    PUT_PTR(HEADER(block_ptr), PACK(size, 0));
    PUT_PTR(FOOTER(block_ptr), PACK(size, 0));
  }

  // Previous block is only free
  else if (!PREV_ALLOC && NEXT_ALLOC) {               
    size += GET_SIZE(HEADER(PREV_BLOCK(block_ptr)));
    block_ptr = PREV_BLOCK(block_ptr);
    removeList(block_ptr);
    PUT_PTR(HEADER(block_ptr), PACK(size, 0));
    PUT_PTR(FOOTER(block_ptr), PACK(size, 0));
  }

  // Both blocks free 
  else if (!PREV_ALLOC && !NEXT_ALLOC) {                
    size += GET_SIZE(HEADER(PREV_BLOCK(block_ptr))) + GET_SIZE(HEADER(NEXT_BLOCK(block_ptr)));
    removeList(PREV_BLOCK(block_ptr));
    removeList(NEXT_BLOCK(block_ptr));
    block_ptr = PREV_BLOCK(block_ptr);
    PUT_PTR(HEADER(block_ptr), PACK(size, 0));
    PUT_PTR(FOOTER(block_ptr), PACK(size, 0));
  }

  // finally insert it...(whew)
  insertList(block_ptr);
  return block_ptr;
}

static void *expandHeap(size_t words) {
  /* Allocate an even number of words to maintain alignment */
  size_t size;
  if (words % 2)
    size = WSIZE * (words+1);
  else
    size = WSIZE * words;

  // Since minimum block size given to us is 4 words (ie 16 bytes)
  if (size < 16)
    size = 16;

  char *block_ptr = mem_sbrk(size);
  if (block_ptr == -1) return NULL;

  // Initialize free block header & footer plus epilogue header
  PUT_PTR(HEADER(block_ptr), PACK(size, 0));
  PUT_PTR(FOOTER(block_ptr), PACK(size, 0));
  PUT_PTR(HEADER(NEXT_BLOCK(block_ptr)), PACK(0, 1));

  // Coalesce block_ptr with surrounding blocks
  return coalesce(block_ptr);
}

// Attempts to fit a block with `asize` bytes
// Returns block addr or NULL if no fit 
static void *fitBlock(size_t asize){
  static int last_size = 0;
  static int loops = 0;

  if(last_size == (int)asize){
      if(loops > 100){  
        int extendsize = MAX(asize, 4 * WSIZE);
        void* block_ptr = expandHeap(extendsize/4);
        return block_ptr;
      }

      loops++;
  }
  else {
    loops = 0;
  }

  void *curr_block = free_list;
  while (GET_ALLOC(HEADER(curr_block)) == 0) {
    size_t header_size = (size_t)GET_SIZE(HEADER(curr_block));
    if (asize <= header_size) {
      last_size = asize;
      return curr_block;
    }

    curr_block = GET_NEXT_PTR(curr_block);
  }

  return NULL;
}

// Place a block of specified size bytes at block_ptr
// Split block if remainder >= min block size
static void place(void *block_ptr, size_t asize){
  size_t size = GET_SIZE(HEADER(block_ptr));
  size_t size_diff = size - asize;

  if (size_diff >= 4 * WSIZE) {
    PUT_PTR(HEADER(block_ptr), PACK(asize, 1));
    PUT_PTR(FOOTER(block_ptr), PACK(asize, 1));

    removeList(block_ptr);
    block_ptr = NEXT_BLOCK(block_ptr);

    PUT_PTR(HEADER(block_ptr), PACK(size-asize, 0));
    PUT_PTR(FOOTER(block_ptr), PACK(size-asize, 0));

    coalesce(block_ptr);
  }
 
  else {
    PUT_PTR(HEADER(block_ptr), PACK(size, 1));
    PUT_PTR(FOOTER(block_ptr), PACK(size, 1));
 
    removeList(block_ptr);
  }
}

// Inserts into the free list
static void insertList(void *block_ptr){
  SET_NEXT_PTR(block_ptr, free_list);
  SET_PREV_PTR(free_list, block_ptr); 
  SET_PREV_PTR(block_ptr, NULL); 

  free_list = block_ptr; 
}

// Remove the free block pointer int the free_list*/
static void removeList(void *block_ptr){
  char *prev = GET_PREV_PTR(block_ptr);
  char *next = GET_NEXT_PTR(block_ptr);

  // If there's a previous, bridge the gap between prev and next
  if (prev != NULL) {
    SET_NEXT_PTR(prev, next);
  }

  // Else just set the head
  else {
    free_list = next;
  }

  // Update previous pointer (if any)
  SET_PREV_PTR(next, prev);
}

// Main malloc functions

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
  char *heap_listp = mem_sbrk(8*WSIZE);
  if (heap_listp == NULL) return -1;

  PUT_PTR(heap_listp, 0);
  PUT_PTR(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
  PUT_PTR(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
  PUT_PTR(heap_listp + (3 * WSIZE), PACK(0, 1));
  free_list = heap_listp + 2*WSIZE;

  if (expandHeap(4) == NULL) return -1;
  return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
  size_t size_adjusted;

  // Include overhead and alignment
  if (size <= DSIZE)
    size_adjusted = DSIZE * 2;
  else
    size_adjusted = DSIZE * ((size + 2*DSIZE - 1) / DSIZE);

  // Is there a place to put it?
  void *block_ptr = fitBlock(size_adjusted);
  if (block_ptr != NULL) {
    place(block_ptr, size_adjusted);
    return (block_ptr);
  }

  // If not, alloc more memory and place block
  size_t extensionSize = MAX(size_adjusted, CHUNKSIZE);
  block_ptr = expandHeap(extensionSize / WSIZE);
  if (block_ptr == NULL)  
    return (NULL);

  place(block_ptr, size_adjusted);
  return (block_ptr);
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
  if (ptr == NULL) return;

  // free and coalesce block
  size_t size = GET_SIZE(HEADER(ptr));
  PUT_PTR(HEADER(ptr), PACK(size, 0));
  PUT_PTR(FOOTER(ptr), PACK(size, 0));

  coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *block_ptr, size_t size) {
  if((int)size < 0) return NULL;

  else if(size == 0) { 
    mm_free(block_ptr); 
    return NULL; 
  } 
  else if(size > 0) {
    size_t old_size = GET_SIZE(HEADER(block_ptr));
    size_t new_size = size + WSIZE*2;

    if(new_size <= old_size) { 
      return block_ptr; 
    }
    else {
      size_t next_alloc = GET_ALLOC(HEADER(NEXT_BLOCK(block_ptr))); 
      size_t size = old_size + GET_SIZE(HEADER(NEXT_BLOCK(block_ptr)));

      if(!next_alloc && size >= new_size) {
        removeList(NEXT_BLOCK(block_ptr)); 
        PUT_PTR(HEADER(block_ptr), PACK(size, 1)); 
        PUT_PTR(FOOTER(block_ptr), PACK(size, 1)); 
        return block_ptr; 
      }
      else {  
        void *new_ptr = mm_malloc(new_size);  
        place(new_ptr, new_size);
        memcpy(new_ptr, block_ptr, new_size); 
        mm_free(block_ptr); 
        return new_ptr; 
      } 
    }
  }

  return NULL;
} 
