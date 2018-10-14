#include <stdio.h>
#include "symbolic_link.h"
#include "absolute_pathname.h"
#include "pathname.h"
#include "filename.h"
#include "inode.h"
#include "file.h"
#include "block.h"

void printsb(block *bp){
    superblock sb;
    memmove(&sb, bp->data, sizeof(superblock));
    printf("superblock of naive_fs\n"
            "size: %d\n"
            "num of data blocks: %d\n"
            "start of data block: %d\n"
            "block number of bitmap: %d\n"
            "block number of inodetable: %d\n"
            "flag: %d\n", 
            sb.size, sb.nblocks, sb.blockstart, sb.bitmap, 
            sb.inodetable, sb.flag);
}

void printbitmap(block *bp){
    char bitmap[BSIZE];
    memmove(bitmap, bp->data, BSIZE);
    int num = 0;
    for(int i = 0; i < 4096; i++){
        if((bitmap[i / 8] & (1 << (i % 8))) != 0)
            num++;
    }
    printf("bitmap usage:\n"
        "%d blocks are used\n"
        "%d are not used\n", 
        num, 4096 - num);
}

void printinodetable(block *bp){
    inodetable itb;
    memmove(&itb, bp->data, sizeof(inodetable));
    int num = 0;
    for(int i = 0; i < NINODE; i++){
        if(itb.tlb[i].type == 0){
            num++;
        }
    }
    printf("inodetable usage:\n"
        "%d inodes are used\n"
        "%d are not used\n",
            (int)NINODE - num, num);
}

int main(){
    //test block layer
    /*
        create file system
        initial lrucache
        create superblock
        create bitmap
        create inodetable
        
    */
    printf("--------------------------block layer-----------------------\n");
    create();
    block bp;
    bp.blockno = 1; readblock(&bp); printsb(&bp);
    bp.blockno = 2; readblock(&bp); printbitmap(&bp);
    bp.blockno = 3; readblock(&bp); printinodetable(&bp);

    //test file and inode layer
    /*
        ialloc an inode
        writedata and readdata in this inode
    */
    printf("-------------------------file layer------------------------\n");
    printf("-------------------------inode layer------------------------\n");
    inode *ino = ialloc(2);
    char in[20] = "Hello world!";
    char out[20];
    int n = writeinode(ino, in, 0, strlen(in));
    printf("write %d bytes in fs, inode num is %d\n", n, ino->inum);
    inodesync(ino);
    n = readinode(ino, out, 0, strlen(in));
    printf("read %d bytes from fs\n", n);
    if(n == strlen(out) && n == strlen(in)){
        printf("input: %s\n",in);
        printf("output: %s\n",out);
    }
    bp.blockno = 3; readblock(&bp); printinodetable(&bp);
    n = readdata(ino->inum, out, 0, strlen(in));
    printf("read %d bytes using inode layer interface\n", n);
    if(n == strlen(out) && n == strlen(in)){
        printf("input: %s\n",in);
        printf("output: %s\n",out);
    }
    n = readdata(ino->inum + 1, out, 0, strlen(in));
    ifree(ino);
    bp.blockno = 3; readblock(&bp); printinodetable(&bp);

    //test filename and pathname layer
    //test absolute pathname layer
    /*
        create root directory
        create directories and files in root directory
        cd in next directory
        ls filenames in current directory

    */
    printf("-------------------------filename layer------------------------\n");
    printf("-------------------------pathname layer------------------------\n");
    printf("-------------------------absolute_pathname layer------------------------\n");
    makeroot();
    printf("root dir inode number is %d\n", rootdirno);
    printf("current work directory is %s, inode number is %d\n", pwd, cwd.inum);
    ls(NULL); //ls / 2files
    mkdir("firstdir");
    touch("firstfile");
    ls(NULL); // ls / 4files
    ls("firstdir"); // ls /firstdir 2files
    cd("firstdir"); // cwd change
    ls(NULL); //ls /firstdir 2files
    cd("/"); //cwd change
    ls(NULL); // ls / 4files
    link("firstfile", "secondfile");
    ls(NULL); // ls / 5files
    link("firstdir", "seconddir"); // can't
    unlink("secondfile"); //unlink
    ls(NULL); //ls / 4files
    bp.blockno = 3; readblock(&bp); printinodetable(&bp);
    m_rename("firstfile", "re_firstfile");
    ls(NULL);
    m_remove("re_firstfile");
    ls(NULL);
    printf("--------------remove directory\n");
    touch("firstdir/aaa");
    ls("firstdir");
    m_remove("firstdir");
    m_remove("firstdir/aaa");
    ls("firstdir");
    m_remove("firstdir");
    ls(NULL);
    bp.blockno = 3; readblock(&bp); printinodetable(&bp);

    printf("-------------------------symbolic layer------------------------\n");
    mkdir("dir");
    ls(NULL);
    mount("dir", "a");
    ls(NULL);
    ls("dir");
    ino = inode_number_to_inode(3);
    n = readinode(ino, out, 0, ino->size);
    out[ino->size] = 0;
    printf("out: %s\n", out);
    cd("a");
    ls(NULL);
    cd("..");
    ls("a");
    umount("a");
    ls(NULL);
    bp.blockno = 3; readblock(&bp); printinodetable(&bp);
    return 0;
}