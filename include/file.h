//
// by msn
//
#ifndef FILE_H
#define FILE_H

#include "common.h"
#include "block.h"
int readinode(inode *ino, char *dst, uint off, uint n);
int writeinode(inode *ino, char *dst, uint off, uint n);
inode *ialloc(uint type);
void ifree(inode *ino);
void inodesync(inode *ino);

#endif //FILE_H