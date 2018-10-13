//
// by msn
//
#ifndef FILENAME_H
#define FILENAME_H
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "inode.h"
#include "pathname.h"

#define DSIZE 12
#define uint uint32_t
typedef struct inode inode;
typedef struct{
    uint inum;
    char name[DSIZE];
}dirent;

inode cwd;
char *pwd;
inode* lookup(char *name, uint dirno);
void dirlink(inode *ino, char *name, uint inum);
void deletelink(inode* ino, char *name);
int dirempty(inode *dir);
#endif //FILENAME_H
