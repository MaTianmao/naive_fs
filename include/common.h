//
// by msn
//

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef unsigned int uint;
#define BSIZE 4096
#define BUFSIZE 4096
#define DIRTY 1
#define VALID 2
#define DIRECT 10
#define SINDIRECT (BSIZE / sizeof(uint))
#define DINDIRECT (BSIZE / sizeof(uint)) * (BSIZE / sizeof(uint))
#define DSIZE 12

struct inode{
    uint inum; //inode number
    int ref; //reference count
    pthread_mutex_t lock; 
    uint size; //file size
    uint type; //0 not alloc 1 directory 2 file 3 sysbolic link
    uint direct[DIRECT + 2]; // 10 direct 1 single indirect
                        // 1 double indirect
};
typedef struct inode inode;

#define NINODE  BSIZE / sizeof(inode)
struct inodetable{
    inode tlb[NINODE];
};
typedef struct inodetable inodetable;

struct block{
    uint blockno;
    size_t size;
    struct block *prev; //LRU cache
    struct block *next;
    uint flag; //dirty
    uint ref; //reference number
    char data[BSIZE];
};
typedef struct block block;

struct cache{
    block buf[BUFSIZE];
    block *head;
};
typedef struct cache cache;

struct superblock{
    uint size; //Size of the blocks
    uint nblocks; // numbers of blocks
    uint ninodes; // numbers of inodes
    uint blockstart; //first block of data block
    uint bitmap; //blockno of bitmap
    uint inodetable; // blockno of inodetable
    uint flag; //Flag to indicate modification of superblock
    pthread_mutex_t lock; //Lock field for free block and free inode lists
};
typedef struct superblock superblock;

struct dirent{
    uint inum;
    char name[DSIZE];
};
typedef struct dirent dirent;



superblock sb;
cache lrucache;
char block_device[BSIZE]; //bitmap
static uint rootdirno = 1;
inode cwd;
char *pwd;
inodetable itlb;

#endif //COMMON_H