#include "inode.h"

inode *inode_number_to_inode(uint inono){
    return &(itlb.tlb[inono]);
}

int readdata(uint inono, char * dst, uint off, uint n){
    inode * ino = inode_number_to_inode(inono);
    if(ino->type == 0){
        printf("can't read an empty inode\n");
        return 0;
    }
    return readinode(ino, dst, off, n);
}

int writedata(uint inono, char * dst, uint off, uint n){
    inode * ino = inode_number_to_inode(inono);
    if(ino->type == 0){
        printf("can't write an empty inode\n");
        return 0;
    }
    return writeinode(ino, dst, off, n);
}

void isync(uint inono){
    inode *ino = inode_number_to_inode(inono);
    if(ino->type == 0){
        printf("can't sync an empty inode\n");
        return;
    }
    inodesync(ino);
}