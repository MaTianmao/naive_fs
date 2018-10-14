#include "symbolic_link.h"
#include <string.h>

void mount(char *path, char *name){
    mkfile(name, 3);
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
        char ct[100];
        strncpy(ct, pwd, strlen(pwd) + 1);
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
    unlink(name);
}