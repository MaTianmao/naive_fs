#include "filename.h"

inode* lookup(char *name, uint dirno){
    inode *dir = inode_number_to_inode(dirno);
    if(dir->type != 1) {
        //printf("333");
        printf("not a directory\n");
        return NULL;
    }
    if(strlen(name) == 0) strncpy(name, ".", 2);
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
    if(lookup(name, ino->inum) != NULL){
        printf("exist, can't link\n");
        return ;
    }
    int off;
    dirent de;
    for(off = 0; off < ino->size; off += sizeof(dirent)){
        if(readinode(ino, (char*)&de, off, sizeof(dirent)) != sizeof(dirent)){
            printf("dirlink read wrong\n");
            return ;
        }
        if(de.inum == 0)
            break;
    }
    strncpy(de.name, name, DSIZE);
    de.inum = inum;
    if(writeinode(ino, (char *)&de, off, sizeof(dirent))!= sizeof(dirent)){
        printf("dirlink write wrong\n");
        return ;
    }
    
}

void deletelink(inode* ino, char *name){
    int off;
    dirent de;
    //printf("unlink %s %d\n",name, strlen(name));
    for(off = 0; off < ino->size; off += sizeof(dirent)){
        if(readinode(ino, (char *)&de, off, sizeof(dirent)) != sizeof(dirent)){
            printf("deletelink read wrong\n");
            return ;
        }
        if(strncmp(name, de.name, DSIZE) == 0){
            de.inum = 0;
            break;
        }
    }
    if(writeinode(ino, (char *)&de, off, sizeof(dirent))!= sizeof(dirent)){
        printf("dirlink write wrong\n");
        return ;
    }
}

int dirempty(inode *dir){
    int off;
    dirent de;
    for(off = 0; off < dir->size; off += sizeof(dirent)){
        if(readinode(dir, (char *)&de, off, sizeof(dirent)) != sizeof(dirent)){
            printf("deletelink read wrong\n");
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