#include "block.h"
#include "inode.h"

inode *inode_number_to_inode(uint inono){
    return &(itlb.tlb[inono]);
}

int readdata(uint inono, char * dst, uint off, uint n){
    inode * ino = inode_number_to_inode(inono);
    return readinode(ino, dst, off, n);
}

int writedata(uint inono, char * dst, uint off, uint n){
    inode * ino = inode_number_to_inode(inono);
    return writeinode(ino, dst, off, n);
}
