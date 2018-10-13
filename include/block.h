//
// by msn
//

#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned int uint;
#define BSIZE 4096
#define BUFSIZE 4096
#define DIRTY 1
#define VALID 2
#include "inode.h"
char block_device[BSIZE]; //bitmap

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

superblock sb;
cache lrucache;

void init();
void create();
void readsb(superblock *sb);
void readbitmap(char *block_device);
void readinodetable(inodetable *itlb);
void writesb(superblock *sb);
void writebitmap(char * block_device);
void writeinodetable(inodetable *itlb);
void readblock(block *bp);
void writeblock(block *bp);
block *getcache(uint blockno);
void freecache(block *bp);

uint balloc();
#endif //BLOCK_H
