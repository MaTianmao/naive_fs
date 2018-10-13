//
// by msn
//

#ifndef INODE_H
#define INODE_H

#include "common.h"
#include "file.h"
int readdata(uint inono, char *dst, uint off, uint n);
inode *inode_number_to_inode(uint inono);
int writedata(uint inono, char *dst, uint off, uint n);
#endif //INODE_H
