//
// by msn
//
#ifndef PATHNAME_H
#define PATHNAME_H
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "filename.h"
#include "absolute_pathname.h"

inode * path_to_inode(char *path, uint dirno);
void link(char *path, char *linkname);
void unlink(char *path);
void mkdir(char *path);
void touch(char *path, uint type = 0);
void remove(char *path);
#endif //PATHNAME_H
