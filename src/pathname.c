#include "pathname.h"

int plain_name(char *path){
    for(int i = 0; path[i] != 0; i++){
        if(path[i] == '/') return 0;
    }
    return 1;
}


inode * path_to_inode(char *path, uint dirno){
    if(strlen(path) == 0) return inode_number_to_inode(1);
    if(plain_name(path))
        return lookup(path, dirno);
    char pa[100];
    strncpy(pa, path, strlen(path) + 1);
    char *first = pa;
    char *rest;
    for(int i = 0; pa[i] != 0; i++){
        if(pa[i] == '/'){
            pa[i] = 0;
            rest = pa + i + 1;
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
    char pa[100];
    strncpy(pa, path, strlen(path) + 1);
    char *first = pa;
    char *rest;
    for(int i = 0; pa[i] != 0; i++){
        if(pa[i] == '/'){
            pa[i] = 0;
            rest = pa + i + 1;
            break;
        }
    }
    inode* ino = lookup(first, dirno);
    uint newdirno = ino->inum;
    next_dir(rest, newdirno, type);
}

void mkfile(char *path, uint type){
    if(*path == '/')
        next_dir(path + 1, rootdirno, type);
    else
        next_dir(path, cwd.inum, type);
}

void mkdir(char *path){
    mkfile(path, 1);
}

void touch(char *path){
    mkfile(path, 2);
}


void handle_linkname(char *path, uint dirno, uint inum, int flag){
    if(plain_name(path)){
        if(flag == 0) dirlink(inode_number_to_inode(dirno), path, inum);
        else if(flag == 1) deletelink(inode_number_to_inode(dirno), path);
        return ;
    }
    char pa[100];
    strncpy(pa, path, strlen(path) + 1);
    char *first = pa;
    char *rest;
    for(int i = 0; pa[i] != 0; i++){
        if(pa[i] == '/'){
            pa[i] = 0;
            rest = pa + i + 1;
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
        printf("can't link dir\n");
        return;
    }
    if(ino->type == 3){
        printf("can't link soft link\n");
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
    }
    else{
        ino = path_to_inode(path, cwd.inum);
    }
    if(ino != NULL){
        //printf("%d\n", ino->inum);
        if(*path == '/')
            handle_linkname(path + 1, rootdirno, 0, 1);
        else
            handle_linkname(path, cwd.inum, 0, 1);
        ino->ref--;
        if(ino->ref == 0){
            ifree(ino);
            printf("unlink successful\n");
        }
    }else{
        printf("not have path %s\n",path);
    }
}

void m_remove(char *path){
    if(strlen(path) == 1 && *path == '/'){
        printf("can't remove root dir");
        return;
    }
    inode *ino;
    if(*path == '/'){
        ino = path_to_inode(path + 1, rootdirno);
    }
    else{
        ino = path_to_inode(path, cwd.inum);
    }
    if(ino == NULL){
        printf("not found this path\n");
        return;
    }
    if(ino->type == 1){
        //printf("1");
        if(dirempty(ino)){
            unlink(path);
            printf("remove dir %s successful\n", path);
        }
        else{
            printf("dir %s not empty, can't remove\n", path);
            return;
        }
    }
    else{
        unlink(path);
        printf("remove file successful\n");
    }
}

void m_rename(char *name1, char *name2){
    int i, j;
    char n1[100], n2[100];
    if(*name1 != '/'){
        strncpy(n1, pwd, strlen(pwd) + 1);
        int len = strlen(n1);
        if(n1[len - 1] != '/'){
            n1[len - 1] = '/';
            n1[len] = 0;
        }
        strncat(n1, name1, strlen(name1));
    }
    else{
        strncpy(n1, name1, strlen(name1) + 1);
    }
    if(*name2 != '/'){
        strncpy(n2, pwd, strlen(pwd) + 1);
        int len = strlen(n2);
        if(n1[len - 1] != '/'){
            n1[len - 1] = '/';
            n1[len] = 0;
        }
        strncat(n2, name2, strlen(name2));
    }
    else{
        strncpy(n2, name2, strlen(name2) + 1);
    }
    for(i = strlen(n1) - 1; i >= 0; i--){
        if(n1[i] == '/') break;
    }
    for(j = strlen(n2) - 1; j >= 0; j--){
        if(n2[j] == '/') break;
    }
    if(i == j && strncmp(n1, n2, i) == 0){
        char *s1 = n1 + i + 1;
        char *s2 = n2 + j + 1;
        n1[i] = 0;
        inode *ino;
        if(*n1 == '/')
            ino = path_to_inode(name1 + 1, rootdirno);
        else if(strlen(n1) > 0)  
            ino = path_to_inode(name1, cwd.inum);
        else{
            ino = inode_number_to_inode(1);
        }
        if(ino != NULL){
            if(ino->type != 1){
                printf("not a dir\n");
                return ;
            }
            else{
                int off;
                dirent de;
                for(off = 0; off < ino->size; off += sizeof(dirent)){
                    if(readinode(ino, (char*)&de, off, sizeof(dirent)) != sizeof(dirent)){
                        printf("dirlink read");
                        return ;
                    }
                    if(de.inum == 0)
                        break;
                    if(strncmp(de.name, s1, DSIZE) == 0){
                        strncpy(de.name, s2, DSIZE);
                        break;
                    }
                }
                if(writeinode(ino, (char *)&de, off, sizeof(dirent))!= sizeof(dirent)){
                    printf("dirlink write wrong\n");
                    return ;
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