/*
 * mm.c
 *
 * Zimo Meng 2100017702
 *
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
// #define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* Global variables */
static char * heap_start = 0; /* Start address of the heap */
static char *heap_listp = 0;  /* Pointer to first block */  
#ifdef NEXT_FIT
static char *rover;           /* Next fit rover */
#endif

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ 
#define DSIZE       8       /* Double word size (bytes) */
#define MIN_FREE_BLOCK_SIZE 16 /* 4(Header size) + 4(PRED pointer size) + 4(SUCC pointer size) + 4(Footer size)*/
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */  
#define SEGREGATED_FREE_LIST_SIZE 14 /* Size of segregated free list */

#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)

/* Pack a size and allocated bit into a word */
#define PACK(size, prev_alloc, alloc)  ((size) | (prev_alloc << 1) |(alloc)) 

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            
#define PUTW(p, val)  (*(unsigned int *)(p) = (val))    
#define PUTD(p, val) (*(unsigned long *)(p) = (val))
/* Get and Set pointer value by ptr at address p */
#define GET_PTR_VAL(p) (*(unsigned long *)(p))
#define SET_PTR(p, ptr)	(*(unsigned long *)(p) = (unsigned long)(ptr))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)                   
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_PREV_ALLOC(p) (GET(p) & 0x2)                    

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)                      
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) 

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) 
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) 

/* Get Real or Relavent address of bp */
#define GET_RELA_ADDRESS(bp) ((unsigned int)((size_t)(bp) - (size_t)(heap_start)))
#define GET_REAL_ADDRESS(bp) ((char *)((size_t)heap_start + (size_t)bp))

/* Read and write pred and succ pointer at address p */
#define GET_PRED(p) (*(unsigned int *)(p))
#define GET_SUCC(p) (*(unsigned int *)(p + WSIZE))
#define GET_PRED_POINTER(p)	(GET_REAL_ADDRESS(GET_PRED(p)))
#define GET_SUCC_POINTER(p)	(GET_REAL_ADDRESS(GET_SUCC(p)))
#define SET_PRED(p, val)	(PUTW(p,val))
#define SET_SUCC(p, val)	(PUTW(p + WSIZE,val))

/* Get the head pointer of the ith list */
#define GET_HDPOINTER(i) (GET_REAL_ADDRESS(i*WSIZE))

/*
 * get_index - return the index of the set.
 */
static inline int get_index(size_t asize){
    //dbg_printf("enter get_index with asize = %lx\n",asize);
    size_t tmp = asize;
    int index = 0;
    while(tmp && index < SEGREGATED_FREE_LIST_SIZE-1){
        tmp = tmp >> 1;
        index ++;
    }
    dbg_printf("leave get_index with asize = %lx, index = %d\n",asize,index);
    return index;
}

/*
 * add_list - add the pointer to the list (LIFO)
 */
static void add_list(void * bp){
    dbg_printf("enter add_list with bp = %lx\n", (size_t)bp);
    int index = get_index(GET_SIZE(HDRP(bp)));dbg_printf("Nothing Wrong in line 115\n");
    char * head_pointer = GET_HDPOINTER(index);dbg_printf("Nothing Wrong in line 116\n");
    char * succ_pointer = GET_REAL_ADDRESS(GET(head_pointer));
    if(GET(head_pointer) == 0){dbg_printf("Nothing Wrong in line 117\n");
        PUTW(head_pointer, GET_RELA_ADDRESS(bp));dbg_printf("Nothing Wrong in line 118\n");
        SET_PRED(bp, GET_RELA_ADDRESS(head_pointer));dbg_printf("Nothing Wrong in line 119\n");
        SET_SUCC(bp, 0);
    }
    else{dbg_printf("Nothing Wrong in line 122\n");
        SET_PRED(bp, GET_RELA_ADDRESS(head_pointer));dbg_printf("Nothing Wrong in line 123\n");
        SET_SUCC(bp, GET_RELA_ADDRESS(GET_PTR_VAL(head_pointer)));dbg_printf("Nothing Wrong in line 124\n");
        SET_PRED(succ_pointer, GET_RELA_ADDRESS(bp));dbg_printf("Nothing Wrong in line 125\n");
        PUTW(head_pointer,GET_RELA_ADDRESS(bp));dbg_printf("Nothing Wrong in line 126\n");
    }
    dbg_printf("leave add_list\n");
}

/*
 * delete_list - delete the pointer out of the list.
 */
static void delete_list(void * bp){
    dbg_printf("enter delete_list with bp = %lx\n",(size_t)bp);
    int index = get_index(GET_SIZE(HDRP(bp)));
    char * head_pointer = GET_HDPOINTER(index);//dbg_printf("Nothing Wrong in line 137\n");
    char * pred_pointer = GET_PRED_POINTER(bp);//dbg_printf("Nothing Wrong in line 138\n");
    char * succ_pointer = GET_SUCC_POINTER(bp);//dbg_printf("Nothing Wrong in line 139\n");
    dbg_printf("%lx,%lx,%lx\n",(size_t)head_pointer,(size_t)pred_pointer,(size_t)succ_pointer);
    if(pred_pointer==head_pointer){//dbg_printf("Nothing Wrong in line 140\n");
        PUTW(head_pointer,GET_RELA_ADDRESS(succ_pointer));//dbg_printf("Nothing Wrong in line 141\n");
        //dbg_printf("%lx\n",(size_t)succ_pointer);
        SET_PRED(succ_pointer,GET_RELA_ADDRESS(pred_pointer));//dbg_printf("Nothing Wrong in line 142\n");
    }
    else{dbg_printf("Nothing Wrong in line 144\n");
        SET_SUCC(pred_pointer,GET_RELA_ADDRESS(succ_pointer));//dbg_printf("Nothing Wrong in line 145\n");
        SET_PRED(succ_pointer,GET_RELA_ADDRESS(pred_pointer));//dbg_printf("Nothing Wrong in line 146\n");
    }
}

/*
 * update_next - update the prev_alloc bit of next block
 */

static void update_next(void * bp, size_t prev_alloc){
    dbg_printf("enter update_next\n");
    char * next_bp = NEXT_BLKP(bp);
    // dbg_printf("%lx,%lx\n",(size_t)next_bp,(size_t)GET_SIZE(HDRP(next_bp)));
    PUTW(HDRP(next_bp),PACK(GET_SIZE(HDRP(next_bp)),prev_alloc,GET_ALLOC(HDRP(next_bp))));
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp) 
{
    dbg_printf("enter coalesce with bp = %lx\n",(size_t)bp);
    size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp)) >> 1;//dbg_printf("Nothing Wrong in line 166\n");
    dbg_printf("%lx + %lx = %lx\n",(size_t)bp,(size_t)GET_SIZE(HDRP(bp)),(size_t)(NEXT_BLKP(bp)));
    // dbg_printf("%lx\n",(size_t)(GET_SIZE(HDRP(NEXT_BLKP(bp)))));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));//dbg_printf("Nothing Wrong in line 167\n");
    size_t size = GET_SIZE(HDRP(bp));//dbg_printf("Nothing Wrong in line 168\n");
    dbg_printf("prev_alloc = %lx, next_alloc = %lx, size = %lx\n",prev_alloc,next_alloc,size);
    if (prev_alloc && next_alloc) {            /* Case 1 */
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        dbg_printf("at case 2\n");
        delete_list((void*)(NEXT_BLKP(bp)));
        PUTW(HDRP(bp), PACK(size, GET_PREV_ALLOC(HDRP(bp)) >> 1, 0));
        PUTW(FTRP(bp), PACK(size, GET_PREV_ALLOC(HDRP(bp)) >> 1, 0));
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        dbg_printf("%lx\n",size);
        dbg_printf("at case 3\n");
        dbg_printf("%lx\n",(size_t)(PREV_BLKP(bp)));
        delete_list((void*)(PREV_BLKP(bp)));
        PUTW(FTRP(bp), PACK(size, GET_PREV_ALLOC(HDRP(PREV_BLKP(bp))) >> 1, 0));
        PUTW(HDRP(PREV_BLKP(bp)), PACK(size, GET_PREV_ALLOC(HDRP(PREV_BLKP(bp))) >> 1, 0));
        bp = PREV_BLKP(bp);
    }

    else {                                     /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
            GET_SIZE(FTRP(NEXT_BLKP(bp)));
        delete_list((void*)(PREV_BLKP(bp)));
        delete_list((void*)(NEXT_BLKP(bp)));
        PUTW(HDRP(PREV_BLKP(bp)), PACK(size, GET_PREV_ALLOC(HDRP(PREV_BLKP(bp))) >> 1, 0));
        PUTW(FTRP(NEXT_BLKP(bp)), PACK(size, GET_PREV_ALLOC(HDRP(PREV_BLKP(bp))) >> 1, 0));
        bp = PREV_BLKP(bp);
    }
    // dbg_printf("%lx,%lx\n",(size_t)(NEXT_BLKP(bp)),(size_t)(GET_SIZE(HDRP(NEXT_BLKP(bp)))));
    update_next(bp, GET_ALLOC(HDRP(bp)));
    // dbg_printf("%lx,%lx\n",(size_t)(NEXT_BLKP(bp)),(size_t)(GET_SIZE(HDRP(NEXT_BLKP(bp)))));
    add_list(bp);
    // dbg_printf("%lx,%lx\n",(size_t)(NEXT_BLKP(bp)),(size_t)(GET_SIZE(HDRP(NEXT_BLKP(bp)))));
#ifdef NEXT_FIT
    /* Make sure the rover isn't pointing into the free block */
    /* that we just coalesced */
    if ((rover > (char *)bp) && (rover < NEXT_BLKP(bp))) 
        rover = bp;
#endif
    dbg_printf("leave coalesce with bp = %lx\n",(size_t)bp);
    dbg_printf("%lx,%lx\n",(size_t)(NEXT_BLKP(bp)),(size_t)(GET_SIZE(HDRP(NEXT_BLKP(bp)))));
    return bp;
}

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
static void * extend_heap(size_t words){
    dbg_printf("enter extend_heap with words = %lx\n",words);
    char *bp;
    size_t size;
    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; dbg_printf("Not Wrong at line 231\n");
    dbg_printf("%lx\n",size);
    if(size < MIN_FREE_BLOCK_SIZE) {dbg_printf("Not Wrong at line 232\n");return NULL;}
    dbg_printf("Not Wrong at line 233\n");
    if ((long)(bp = mem_sbrk(size)) == -1){
        dbg_printf("Not Wrong at line 234\n");
        return NULL;                                        
    }
    dbg_printf("Not Wrong at line 235\n");
    dbg_printf("%lx\n",(size_t)PREV_BLKP(bp));
    /* Initialize free block header/footer and the epilogue header */
    PUTW(HDRP(bp), PACK(size,GET_PREV_ALLOC(HDRP(bp)) >> 1,0));dbg_printf("Not Wrong at line 237\n");         /* Free block header */   
    PUTW(FTRP(bp), PACK(size,GET_PREV_ALLOC(HDRP(bp)) >> 1,0));dbg_printf("Not Wrong at line 238\n");         /* Free block footer */ 
    SET_PRED(bp,0);dbg_printf("Not Wrong at line 239\n");
    SET_SUCC(bp,0);dbg_printf("Not Wrong at line 240\n");
    PUTW(HDRP(NEXT_BLKP(bp)), PACK(0,0,1));dbg_printf("Not Wrong at line 241\n"); /* New epilogue header */ 
    dbg_printf("leave extend_heap\n");
    /* Coalesce if the previous block was free */
    return coalesce(bp);    
}

/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
static void *find_fit(size_t asize)
{
    dbg_printf("enter find_fit with asize = %lx\n",asize);
#ifdef NEXT_FIT 
    /* Next fit search */
    char *oldrover = rover;

    /* Search from the rover to the end of list */
    for ( ; GET_SIZE(HDRP(rover)) > 0; rover = NEXT_BLKP(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    /* search from start of list to old rover */
    for (rover = heap_listp; rover < oldrover; rover = NEXT_BLKP(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    return NULL;  /* no fit found */
#else 
    /* First-fit search */
    void * bp = NULL;
    int index = get_index(asize);
    dbg_printf("Not Wrong at line 257\n");
    int cur_block = GET(GET_HDPOINTER(index));
    dbg_printf("Not Wrong at line 259\n");
    while(bp == NULL && index < SEGREGATED_FREE_LIST_SIZE){
        for(;cur_block!=0;cur_block=GET_SUCC(GET_REAL_ADDRESS(cur_block))){
            if(asize <= GET_SIZE(HDRP(GET_REAL_ADDRESS(cur_block)))){
                bp = GET_REAL_ADDRESS(cur_block);
                dbg_printf("leave find_fit with bp = %lx,index = %d\n",(size_t)bp,index);
                return bp;
            }
        }
        cur_block = GET(GET_HDPOINTER(++index));
    }
    dbg_printf("leave find_fit with bp = NULL\n");
    return NULL; /* No fit */
#endif
}

/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp, size_t asize)
{
    dbg_printf("enter place with bp = %lx, asize = %lx\n",(size_t)bp,asize);
    size_t csize = GET_SIZE(HDRP(bp));   
    dbg_printf("csize = %lx\n",csize);
    delete_list(bp);
    if ((csize - asize) >= (2*DSIZE)) { 
        PUTW(HDRP(bp), PACK(asize, GET_PREV_ALLOC(HDRP(bp)) >> 1, 1));
        PUTW(HDRP(NEXT_BLKP(bp)), PACK(csize - asize, 1, 0));
        PUTW(FTRP(NEXT_BLKP(bp)), PACK(csize - asize, 1, 0));
        add_list(NEXT_BLKP(bp));
    }
    else { 
        PUTW(HDRP(bp), PACK(csize, GET_PREV_ALLOC(HDRP(bp)) >> 1, 1));
    }
    update_next(bp, GET_ALLOC(HDRP(bp)));
    dbg_printf("leave place\n");
}

/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void) {
    dbg_printf("enter mm_init\n");
        /* Create the initial empty heap */
    if ((heap_start = mem_sbrk(4*WSIZE + SEGREGATED_FREE_LIST_SIZE * WSIZE)) == (void *)-1) 
        return -1;
    for(int i=0;i<SEGREGATED_FREE_LIST_SIZE;++i){
        PUTW(heap_start + i * WSIZE, 0);
    }
    heap_listp = heap_start + SEGREGATED_FREE_LIST_SIZE * WSIZE;
    PUTW(heap_listp, 0);                          /* Alignment padding */
    PUTW(heap_listp + (1*WSIZE), PACK(DSIZE, 0, 1)); /* Prologue header */ 
    PUTW(heap_listp + (2*WSIZE), PACK(DSIZE, 0, 1)); /* Prologue footer */ 
    PUTW(heap_listp + (3*WSIZE), PACK(0, 1, 1));     /* Epilogue header */
    heap_listp += (2*WSIZE);                     

#ifdef NEXT_FIT
    rover = heap_listp;
#endif

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) 
        return -1;
    dbg_printf("leave mm_init with heap_start = %lx, heap_listp = %lx\n",(size_t)heap_start,(size_t)heap_listp);
    return 0;
}

/*
 * malloc
 */
void *malloc (size_t size) {
    dbg_printf("enter malloc with size = %lx\n",size);
    mm_checkheap(352);
    size_t asize;
    size_t extend_size;
    void * bp = NULL;

    /* Ignore spurious requests */
    if(size <= 0){
        return NULL;
    }

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= 3*WSIZE)                                          
        asize = 2*DSIZE;                                        
    else
        asize = DSIZE * ((size + WSIZE + (DSIZE - 1)) / DSIZE); 

    if((bp = find_fit(asize)) != NULL){
        place(bp, asize);
        dbg_printf("leave malloc with bp = %lx, payload size = %x\n----------------------\n",(size_t)bp,GET_SIZE(HDRP(bp)));
        return bp;
    }
    extend_size = MAX(asize, CHUNKSIZE);dbg_printf("Not Wrong at line 367\n");
    if((bp = extend_heap(extend_size / WSIZE))==NULL){dbg_printf("Not Wrong at line 368\n");
        return NULL;
    }
    place(bp, asize);
    dbg_printf("leave malloc with bp = %lx, payload size = %x\n----------------------\n",(size_t)bp,GET_SIZE(HDRP(bp)));
    return bp;
}

/*
 * free
 */
void free (void *ptr) {
    dbg_printf("enter free with ptr = %lx\n",(size_t)ptr);
    // dbg_printf("next block is %lx\n",(size_t)(NEXT_BLKP(ptr)));
    if (ptr == 0)return;
    if(GET_ALLOC(HDRP(ptr))==0) 
    return;
    size_t size = GET_SIZE(HDRP(ptr));
    if (heap_listp == 0){dbg_printf("Nothing Wrong in line 369\n");
        mm_init();dbg_printf("Nothing Wrong in line 370\n");
    }
    PUTW(HDRP(ptr), PACK(size, GET_PREV_ALLOC(HDRP(ptr)) >> 1, 0));dbg_printf("Nothing Wrong in line 373\n");
    PUTW(FTRP(ptr), PACK(size, GET_PREV_ALLOC(HDRP(ptr)) >> 1, 0));dbg_printf("Nothing Wrong in line 374\n");
    coalesce(ptr);
    dbg_printf("leave free\n----------\n");
}

/*
 * realloc - you may want to look at mm-naive.c
 */
void *realloc(void *oldptr, size_t size) {
    dbg_printf("enter realloc\n");
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) {
        free(oldptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if(oldptr == NULL) {
        return malloc(size);
    }

    newptr = malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if(!newptr) {
        return 0;
    }

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(oldptr));
    if(size < oldsize) oldsize = size;
    memcpy(newptr, oldptr, oldsize);

    /* Free the old block. */
    free(oldptr);

    return newptr;
}

/*
 * calloc - you may want to look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void *calloc (size_t nmemb, size_t size) {
    size_t bytes = nmemb * size;
    void *newptr;

    newptr = malloc(bytes);
    memset(newptr, 0, bytes);

    return newptr;
}


/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void *p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
static int aligned(const void *p) {
    return (size_t)ALIGN(p) == (size_t)p;
}

/*
 * mm_checkheap
 */
void mm_checkheap(int lineno) {
    char * pr = heap_listp + 2 * WSIZE;
    char * ep = (char*)mem_heap_hi() + 1; 
    if(GET(pr)!=11||GET(pr-WSIZE)!=11){
        dbg_printf("Prologue block error at line %d\n",lineno);
        exit(1);
    }
    if(GET_ALLOC(ep-WSIZE)!=1){
        dbg_printf("Epilogue block error at line %d\n",lineno);
        exit(1);
    }
    int alloc = 1;
    int num_free_1 = 0, num_free_2 = 0;
    for(char * cur_block = NEXT_BLKP(pr);cur_block!=ep;cur_block = NEXT_BLKP(cur_block)){
        if(!aligned(cur_block)){
            dbg_printf("Address aligned error at address %lx, line %d\n",(size_t)cur_block,lineno);
            exit(1);
        }
        int size = GET_SIZE(HDRP(cur_block));
        if(size % DSIZE !=0){
            dbg_printf("Size alignment error at address %lx, line %d\n",(size_t)cur_block,lineno);
            exit(1);
        }
        int cur_alloc = GET_ALLOC(HDRP(cur_block));
        if(cur_alloc == 0){
            ++ num_free_1;
            if(alloc == 0){
                dbg_printf("Coalesce error at address %lx, line %d\n",(size_t)cur_block,lineno);
                exit(1);
            }
            if(GET(HDRP(cur_block))!=GET(FTRP(cur_block))){
                dbg_printf("Header and footer consistency error at address %lx, line %d\n",(size_t)cur_block,lineno);
            }
        }
        int prev_alloc = GET_PREV_ALLOC(HDRP(cur_block)) >> 1;
        if(prev_alloc != alloc){
            dbg_printf("Prev_alloc bit consistency error at address %lx, line %d\n",(size_t)cur_block,lineno);
            exit(1);
        }
        alloc = cur_alloc;
    }
    for(int i=0;i<SEGREGATED_FREE_LIST_SIZE;++i){
        char* bp = GET_REAL_ADDRESS((GET(heap_start+i*WSIZE)));
        if(bp!=heap_start&&(bp<pr||bp>ep)){
            dbg_printf("the %d th list head error at line %d\n",i,lineno);
            exit(1);            
        }
    }
    for(int i=0;i<SEGREGATED_FREE_LIST_SIZE;++i){
        int cur_block = GET(GET_HDPOINTER(i));
        int prev = i * WSIZE;
        int succ = GET(GET_HDPOINTER(i));
        for(;cur_block!=0;cur_block=GET_SUCC(GET_REAL_ADDRESS(cur_block))){
            ++num_free_2;
            int cur_prev = GET_PRED(GET_REAL_ADDRESS(cur_block));
            int size = GET_SIZE(HDRP(GET_REAL_ADDRESS(cur_block)));
            if(succ!=cur_block||prev!=cur_prev){
                dbg_printf("Next/prev pointers consistency error at address %lx, line %d, list %d\n",(size_t)(GET_REAL_ADDRESS(cur_block)),lineno,i);
                exit(1);
            }
            if(!in_heap(GET_REAL_ADDRESS(cur_block))){
                dbg_printf("In-heap error at address %lx, line %d, list %d\n",(size_t)(GET_REAL_ADDRESS(cur_block)),lineno,i);
                exit(1);
            }
            if(i < SEGREGATED_FREE_LIST_SIZE-1 && size > (1 << i)){
                dbg_printf("Range error at address %lx, line %d, list %d\n",(size_t)(GET_REAL_ADDRESS(cur_block)),lineno,i);
            }
            prev = cur_block;
            succ = GET_SUCC(GET_REAL_ADDRESS(cur_block));
        }
    }
}
