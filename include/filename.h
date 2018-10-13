//
// by msn
//
#ifndef FILENAME_H
#define FILENAME_H

#include "common.h"
#include "inode.h"

inode* lookup(char *name, uint dirno);
void dirlink(inode *ino, char *name, uint inum);
void deletelink(inode* ino, char *name);
int dirempty(inode *dir);
#endif //FILENAME_H