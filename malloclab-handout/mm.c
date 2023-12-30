/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>


#include "mm.h"
#include "memlib.h"
/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
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

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)
#define MAX(x,y) ((x)>(y)?(x):(y))
#define PACK(size,alloc) ((size)|(alloc))
#define GET(p) (*(unsigned int*)(p))
#define PUT(p,val) (*(unsigned int*)(p)=(val))
#define GET_SIZE(p) (GET(p)&~0x7)
#define GET_ALLOC(p) (GET(p)&0x1)
#define HDRP(bp) ((char*)(bp)-WSIZE)
#define FTRP(bp) ((char*)(bp)+GET_SIZE(HDRP(bp))-DSIZE)
#define NEXT_BLKP(bp) ((char*)(bp)+GET_SIZE(((char*)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char*)(bp)-GET_SIZE(((char*)(bp)-DSIZE)))
#define NEXT_NODE(bp) (*((unsigned int*)(bp)+1)) 
#define PREV_NODE(bp) (*(unsigned int*)(bp)) 
#define GET_NEXT(bp) ((unsigned int*)(bp)+1)
#define GET_PREV(bp) (unsigned int*)(bp)
#define CLASSNUM 20

static char *heap_listp;
int mm_init(void);
static void*extend_heap(size_t words);
static void *coalesce(void *bp);
static void insert(void *bp);
static void delete(void *bp);
static int search(size_t size);
static void space(void*bp,size_t size);
static void *first_fit(size_t size);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    
    if((heap_listp=mem_sbrk((CLASSNUM+4)*WSIZE))==(void*)-1) return -1;
    for(int i=0;i<CLASSNUM;i++){
        PUT(heap_listp+i*WSIZE,NULL);
    }
    PUT(heap_listp+CLASSNUM*WSIZE,0);
    PUT(heap_listp+(CLASSNUM+1)*WSIZE,PACK(DSIZE,1));
    PUT(heap_listp+(CLASSNUM+2)*WSIZE,PACK(DSIZE,1));
    PUT(heap_listp+(CLASSNUM+3)*WSIZE,PACK(0,1));
    if(extend_heap(CHUNKSIZE/WSIZE)==NULL) return -1;
    return 0;
}

/* 
 * extend_headp.
 */
static void*extend_heap(size_t words){
    char *bp;
    size_t size;
    size = (words%2)?(words+1)*WSIZE:words*WSIZE;
    #ifdef test
    printf("extend size %d\n",size);
    #endif
    if((bp=mem_sbrk(size))==(void*)-1) return NULL;
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));
    return coalesce(bp);
}
/* 
 * coalesce.
 */
static void *coalesce(void *bp){
    if(bp==NULL) return;
    size_t prev_alloc=GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc=GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size=GET_SIZE(HDRP(bp));
    if(prev_alloc&&next_alloc){}
    else if(prev_alloc&&!next_alloc){
        size+=GET_SIZE(HDRP(NEXT_BLKP(bp)));
        delete(NEXT_BLKP(bp));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0));
    }
    else if(!prev_alloc&&next_alloc){
        size+=GET_SIZE(FTRP(PREV_BLKP(bp)));
        delete(PREV_BLKP(bp));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(PREV_BLKP(bp)),PACK(size,0));
        bp=PREV_BLKP(bp);
    }
    else{
        size+=GET_SIZE(HDRP(NEXT_BLKP(bp)))+GET_SIZE(FTRP(PREV_BLKP(bp)));
        delete(PREV_BLKP(bp));
        delete(NEXT_BLKP(bp));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(PREV_BLKP(bp)),PACK(size,0));
        bp=PREV_BLKP(bp);
    }
    insert(bp);
    return bp;
}
/* 
 * insert.
 */
static void insert(void *bp){
    int num=search(GET_SIZE(HDRP(bp)));
    void *next=GET(heap_listp+num*WSIZE);
    PUT(GET_PREV(bp),NULL);
    PUT(GET_NEXT(bp),NULL);
    if(next==NULL){
        PUT(heap_listp+num*WSIZE,bp);
    }
    else{
        PUT(GET_NEXT(bp),next);
        PUT(GET_PREV(next),bp);
        PUT(heap_listp+num*WSIZE,bp);
    }
}
/* 
 * delete.
 */
static void delete(void *bp){
    void*prev_bp=PREV_NODE(bp);
    void*next_bp=NEXT_NODE(bp);

    int num=search(GET_SIZE(HDRP(bp)));
    if((prev_bp==NULL)&&(next_bp==NULL)){
        PUT(heap_listp+num*WSIZE,NULL);
    }
    else if((prev_bp==NULL)&&(next_bp!=NULL)){
        PUT(heap_listp+num*WSIZE,next_bp);
        PUT(GET_PREV(next_bp),NULL);
    }
    else if((prev_bp!=NULL)&&(next_bp==NULL)){
        PUT(GET_NEXT(prev_bp),NULL);
    }
    else{
        PUT(GET_NEXT(prev_bp),next_bp);
        PUT(GET_PREV(next_bp),prev_bp);
    }
}
/* 
 * search.
 */
static int search(size_t size){
    int i;
    for(i=WSIZE;i<(CLASSNUM+WSIZE-1);i++){
        if(size<=(1<<i)) break;
    }
    return i-4;
}
/* 
 * space."size"includes the head and foot.
 */
static void space(void*bp,size_t size){           
    size_t sub_size=GET_SIZE(HDRP(bp))-ALIGN(size);
    if(ALIGN(size)>GET_SIZE(HDRP(bp))) return;
    else if((ALIGN(size)+4*WSIZE)<=GET_SIZE(HDRP(bp))){
        delete(bp);
        PUT(HDRP(bp),PACK(ALIGN(size),1));
        PUT(FTRP(bp),PACK(ALIGN(size),1));
        PUT(HDRP(NEXT_BLKP(bp)),PACK(sub_size,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(sub_size,0));
        insert(NEXT_BLKP(bp));
    }
    else{
        delete(bp);
        PUT(HDRP(bp),PACK(GET_SIZE(HDRP(bp)),1));
        PUT(FTRP(bp),PACK(GET_SIZE(HDRP(bp)),1));
        
    }
    /*ps:these two line should not be placed in start of the function,
    because it set NEXT_NODE(bp)=PREV_NODE(bp)=NULL resulting 
    that fuction delete makes mistakes*/    
    PUT(GET_PREV(bp),NULL);
    PUT(GET_NEXT(bp),NULL);
}

/* 
 * first_fit."size"includes the head and foot.
 */

static void *first_fit(size_t size){
    int num=search(size);
    void *bp;
    while(num<=CLASSNUM){
        /*I firstly set bp=GET(heap_listp+WSIZE) that leads to bug*/
        for(bp=GET(heap_listp+num*WSIZE);bp!=NULL;bp=NEXT_NODE(bp)){
            if(size<=GET_SIZE(HDRP(bp))){
                space(bp,size);
                return (void*)bp;
            }
        }
        num++;
    }
    return NULL;
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t newsize = ALIGN(size)+DSIZE;
    char *bp;
    if((bp=first_fit(newsize))!=NULL){
        return bp;
    }
    size_t extendsize=MAX(newsize,CHUNKSIZE);
    if((bp=extend_heap(extendsize/WSIZE))==NULL) return NULL;
    space(bp,newsize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if(ptr==NULL) return;
    size_t size=GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr),PACK(size,0));
    PUT(FTRP(ptr),PACK(size,0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL) 
        return NULL;
    copySize = (size_t)(GET_SIZE(HDRP(oldptr))-DSIZE);
    if (size < copySize) 
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}