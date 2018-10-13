//
// by msn
//
#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "block.h"
#include "inode.h"
#define DIRECT 10
#define SINDIRECT (BSIZE / sizeof(uint))
#define DINDIRECT (BSIZE / sizeof(uint)) * (BSIZE / sizeof(uint))
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

int readinode(inode *ino, char *dst, uint off, uint n);
int writeinode(inode *ino, char *dst, uint off, uint n);
inode *ialloc(uint type);
void ifree(inode *ino);
#endif //FILE_H