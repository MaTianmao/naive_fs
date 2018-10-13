//
// by msn
//

#ifndef INODE_H
#define INODE_H
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "file.h"
#include "filename.h"

#define NINODE  BSIZE / sizeof(inode)
typedef struct{
    inode tlb[NINODE];
}inodetable;

inodetable itlb;

int readdata(uint inono, char *dst, uint off, uint n);
inode *inode_number_to_inode(uint inono);
int writedata(uint inono, char *dst, uint off, uint n);
#endif //INODE_H
