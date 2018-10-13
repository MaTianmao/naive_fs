#include "filename.h"

inode* lookup(char *name, uint dirno){
    inode *dir = inode_number_to_inode(dirno);
    if(dir->type != 0) {
        printf("not a directory");
        return NULL;
    }
    dirent dd;
    for(uint off = 0; off <= dir->size; off += sizeof(dirent)){
        readinode(dir, (char *)&dd, off, sizeof(dirent));
        if(dd.inum == 0)
            continue; 
        if(strncmp(name, dd.name, DSIZE) == 0){
            return inode_number_to_inode(dd.inum);
        }
    }
    return NULL;
}

void dirlink(inode *ino, char *name, uint inum){
    if(lookup(name, ino->inum)!=NULL){
        printf("exist");
        return ;
    }
    int off;
    dirent de;
    for(off = 0; off < ino->size; off += sizeof(dirent)){
        if(readinode(ino, (char*)&de, off, sizeof(dirent)) != sizeof(dirent)){
            printf("dirlink read");
            return ;
        }
        if(de.inum == 0)
            break;
    }
    strncpy(de.name, name, DSIZE);
    de.inum = inum;
    if(writeinode(ino, (char *)&de, off, sizeof(dirent))!= sizeof(dirent)){
        printf("dirlink write");
        return ;
    }
    
}

void deletelink(inode* ino, char *name){
    int off;
    dirent de;
    for(off = 0; off < ino->size; off += sizeof(dirent)){
        if(readinode(ino, (char *)&de, off, sizeof(dirent)) != sizeof(dirent)){
            printf("deletelink read");
            return ;
        }
        if(strncmp(name, de.name, DSIZE) == 0){
            de.inum = 0;
        }
    }
}

int dirempty(inode *dir){
    int off;
    dirent de;
    for(off = 0; off < dir->size; off += sizeof(dirent)){
        if(readinode(dir, (char *)&de, off, sizeof(dirent)) != sizeof(dirent)){
            printf("deletelink read");
            return 0;
        }
        if(de.inum != 0){
            if(strncmp(de.name, "..", DSIZE) == 0)
                continue;
            if(strncmp(de.name, ".", DSIZE) == 0)
                continue;
            return 0;
        }
    }
    return 1;
}