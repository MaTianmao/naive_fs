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
#include <stdint.h>
#define uint uint32_t
#define remove(x) m_remove(x)
#define rename(x) m_rename(x)
typedef struct inode inode;
inode * path_to_inode(char *path, uint dirno);
void link(char *path, char *linkname);
void unlink(char *path);
void mkdir(char *path);
void touch(char *path, uint type);
void m_remove(char *path);
#endif //PATHNAME_H
