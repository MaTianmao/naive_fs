//
// by msn
//
#ifndef PATHNAME_H
#define PATHNAME_H

#include "common.h"
#include "filename.h"

inode * path_to_inode(char *path, uint dirno);
void link(char *path, char *linkname);
void unlink(char *path);
void mkdir(char *path);
void touch(char *path, uint type);
void m_remove(char *path);
void m_rename(char *name1, char *name2);
#endif //PATHNAME_H
