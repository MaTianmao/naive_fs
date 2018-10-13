#include "absolute_pathname.h"

void makeroot(){
    inode *ino = ialloc(1);
    ino->ref = 1;
    dirlink(ino, ".", ino->inum);
    dirlink(ino, "..", ino->inum);
    rootdirno = ino->inum;
}

void cd(char *path){
    inode *ino;
    if(*path == '/'){
        ino = path_to_inode(path + 1, rootdirno);
    }
    else{
        ino = path_to_inode(path, cwd.inum);
    }
    if(ino != NULL){
        if(ino->type != 1){
            printf("not a dir");
            return;
        }
        cwd = *ino;
        if(*path == '/'){
            pwd = path;
        }else{
            int len = strlen(pwd);
            if(pwd[len - 1] == '/'){
                pwd[len - 1] = '/';
                pwd[len] = 0;
            }
            strncat(pwd, path, strlen(path));
        }
        printf("pwd %s\n", pwd);
    }
    else{
        printf("can't cd in %s\n",path);
    }
}

void ls(char *path){
    inode *ino;
    if(*path){
        ino = path_to_inode(path + 1, rootdirno);
    }
    else{
        ino = path_to_inode(path, cwd.inum);
    }
    if(ino != NULL){
        if(ino->type != 1){
            printf("not a dir");
        }
        else{
            dirent de;
            for(int off = 0; off < ino->size; off += sizeof(dirent)){
                if(readinode(ino, (char *)&de, off, sizeof(dirent)) != sizeof(dirent)){
                    printf("ls read dirent wrong\n");
                    return;
                }
                if(de.inum != 0){
                    printf("%s\n", de.name);
                }
            }
        }
    }else{
        printf("not have this dir");
    }
}
