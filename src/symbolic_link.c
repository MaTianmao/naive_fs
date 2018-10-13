#include "symbolic_link.h"
#include <string.h>

void mount(char *path, char *name){
    touch(name, 3);
    inode *ino;
    if(*name == '/')
        ino = lookup(name + 1, rootdirno);
    else
        ino = lookup(name, cwd.inum);
    if(ino == NULL){
        printf("not found %s\n", name);
        return;
    }
    if(*path == '/'){
        writeinode(ino, path, 0, strlen(path));
    }
    else{
        char *ct;
        strncpy(ct, pwd, strlen(pwd));
        int len = strlen(ct);
        if(ct[len - 1] == '/')
            strncat(ct, path, strlen(path));
        else{
            ct[len - 1] = '/';
            ct[len] = 0;
            strncat(ct, path, strlen(path));
        }
        writeinode(ino, ct, 0, strlen(ct));
    }
}

void umount(char *name){
    inode *ino;
    if(*name == '/'){
        ino = lookup(name + 1, rootdirno);
    }
    else{
        ino = lookup(name, cwd.inum);
    }
    if(ino == NULL){
        printf("not found %s\n", name);
        return;
    }
    if(ino->type == 3){
        ifree(ino);
        printf("umount symbolic link successful\n");
    }
    else{
        printf("not a symbolic link\n");
    }
}