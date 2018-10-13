//
// by msn
//

#ifndef BLOCK_H
#define BLOCK_H

#include "common.h"

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