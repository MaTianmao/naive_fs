#include "pathname.h"

int plain_name(char *path){
    for(int i = 0; path[i] != 0; i++){
        if(path[i] == '/') return 0;
    }
    return 1;
}


inode * path_to_inode(char *path, uint dirno){
    if(plain_name(path))
        return lookup(path, dirno);
    char *first = path;
    char *rest;
    for(int i = 0; path[i] != 0; i++){
        if(path[i] == '/'){
            path[i] = 0;
            rest = path[i+1];
            break;
        }
    }
    inode* ino = lookup(first, dirno);
    uint newdirno = ino->inum;
    return path_to_inode(rest, newdirno);
}
void createdir(char *name, uint dirno){
    inode *dno = inode_number_to_inode(dirno);
    inode *ino = ialloc(1);
    ino->ref = 1;
    dno->ref ++;
    dirlink(ino, ".", ino->inum);
    dirlink(ino, "..", dno->inum);
    dirlink(dno, name, ino->inum);
}

void createfile(char *name, uint dirno){
    inode *dno = inode_number_to_inode(dirno);
    inode *ino = ialloc(2);
    ino->ref = 1;
    dirlink(dno, name, ino->inum);
}

void createsoft(char *name, uint dirno){
    inode *dno = inode_number_to_inode(dirno);
    inode *ino = ialloc(3);
    ino->ref = 1;
    dirlink(dno, name, ino->inum);
}


void next_dir(char *path, uint dirno, uint type){
    if(plain_name(path)){
        if(type == 1) createdir(path, dirno);
        if(type == 2) createfile(path, dirno);
        if(type == 3) createsoft(path, dirno);
        return ;
    }
    char *first = path;
    char *rest;
    for(int i = 0; path[i] != 0; i++){
        if(path[i] == '/'){
            path[i] = 0;
            rest = path[i+1];
            break;
        }
    }
    inode* ino = lookup(first, dirno);
    uint newdirno = ino->inum;
    next_dir(rest, newdirno, type);
}

void mkdir(char *path){
    inode *ino;
    if(*path == '/')
        next_dir(path + 1, rootdirno, 1);
    else
        next_dir(path, cwd.inum, 1);
}
void touch(char *path, uint type){
    inode *ino;
    if(*path == '/')
        next_dir(path + 1, rootdirno, type);
    else
        next_dir(path, cwd.inum, type);
}

void handle_linkname(char *path, uint dirno, uint inum, int flag){
    if(plain_name(path)){
        if(flag == 0) dirlink(inode_number_to_inode(dirno), path, inum);
        else if(flag == 1) deletelink(inode_number_to_inode(dirno), path);
        return ;
    }
    char *first = path;
    char *rest;
    for(int i = 0; path[i] != 0; i++){
        if(path[i] == '/'){
            path[i] = 0;
            rest = path[i+1];
            break;
        }
    }
    inode* ino = lookup(first, dirno);
    uint newdirno = ino->inum;
    handle_linkname(rest, newdirno, inum, flag);
}

void link(char *path, char *linkname){
    inode *ino;
    if(*path == '/'){
        ino = path_to_inode(path + 1, rootdirno);
    }
    else{
        ino = path_to_inode(path, cwd.inum);
    }
    if(ino == NULL){
        printf("not have the path");
        return ;
    }
    if(ino->type == 1){
        printf("can't link dir");
        return;
    }
    if(*linkname == '/') 
        handle_linkname(linkname + 1, rootdirno, ino->inum, 0);
    else
        handle_linkname(linkname, cwd.inum, ino->inum, 0);
    ino->ref++;
}

void unlink(char *path){
    inode *ino;
    if(*path == '/'){
        ino = path_to_inode(path + 1, rootdirno);
        handle_linkname(path + 1, rootdirno, 0, 1);
    }
    else{
        ino = path_to_inode(path, cwd.inum);
        handle_linkname(path, cwd.inum, 0, 1);
    }
    if(ino != NULL){
        ino->ref--;
        if(ino->ref == 0){
            ifree(ino);
            printf("unlink successful\n");
        }
    }else{
        printf("not have path %s\n",path);
    }
}

void remove(char *path){
    if(strlen(path) == 1 && *path == '/'){
        printf("can't remove root dir");
        return;
    }
    inode *ino;
    if(*path){
        ino = path_to_inode(path + 1, rootdirno);
    }
    else{
        ino = path_to_inode(path, cwd.inum);
    }
    if(ino->type == 1){
        if(dirempty(ino)){
            unlink(path);
            printf("remove dir successful");
        }
    }
    else{
        unlink(path);
        printf("remove file successful");
    }
}

void rename(char *name1, char *name2){
    int i, j;
    for(i = strlen(name1) - 1; i >= 0; i--){
        if(name1[i] == '/') break;
    }
    for(j = strlen(name2) - 1; j >= 0; j--){
        if(name2[j] == '/') break;
    }
    if(i == j && strncmp(name1, name2, i) == 0){
        char *s1 = name1[i + 1];
        char *s2 = name2[j + 1];
        name1[i] = 0;
        inode *ino;
        if(*name1 == '/')
            ino = path_to_inode(name1 + 1, rootdirno);
        else    
            ino = path_to_inode(name1, cwd.inum);
        if(ino != NULL){
            if(ino->type != 1){
                printf("not a dir\n");
                return ;
            }
            else{
                dirent de;
                for(int off = 0; off < ino->size; off += sizeof(dirent)){
                    if(readinode(ino, (char*)&de, off, sizeof(dirent)) != sizeof(dirent)){
                        printf("dirlink read");
                        return ;
                    }
                    if(de.inum == 0)
                        break;
                    if(strncmp(de.name, s1, DSIZE) == 0){
                        strncpy(de.name, s2, DSIZE);
                        return;
                    }
                }
            }
        }
        else{
            printf("not have this dir\n");
            return;
        }
    }
    else{
        printf("not same path\n");
    }
}