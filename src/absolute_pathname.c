#include "absolute_pathname.h"

void makeroot(){
    inode *ino = ialloc(1);
    ino->ref = 1;
    dirlink(ino, ".", ino->inum);
    dirlink(ino, "..", ino->inum);
    rootdirno = ino->inum;
    cwd = *ino; 
    strncpy(pwd, "/", 2);
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
        if(ino->type == 3){
            char s[100];
            int n = readinode(ino, s, 0, ino->size);
            s[ino->size] = 0;
            //printf("s: %s\n", s);
            cd(s);
            return;
        }
        if(ino->type != 1){
            printf("cd not a dir\n");
            return;
        }
        cwd = *ino;
        //printf("%s\n", path);
        if(strncmp(path, "..", 2) == 0){
            for(int i = strlen(pwd) - 1; i >=0; i--){
                if(pwd[i] == '/'){
                    pwd[i + 1] = 0;
                    break;
                }
            }
        }
        else{
            if(*path == '/'){
                strncpy(pwd, path, strlen(path) + 1);
            }else{
                int len = strlen(pwd);
                if(pwd[len - 1] != '/'){
                    pwd[len] = '/';
                    pwd[len + 1] = 0;
                }
                strncat(pwd, path, strlen(path));
            }
        }
        printf("cwd change to %s\n", pwd);
    }
    else{
        printf("can't cd in %s\n",path);
    }
}

void ls(char *path){
    char pa[100];
    //printf("pwd %s\n", pwd);
    if(path == NULL){
        strncpy(pa, pwd, strlen(pwd) + 1);
        path = pa;
    }
    inode *ino;
    printf("ls: files in the %s\n", path);
    if(*path == '/'){
        //printf("absolute\n");
        ino = path_to_inode(path + 1, rootdirno);
        //printf("inode number %d\n", ino->inum);
    }
    else{
        ino = path_to_inode(path, cwd.inum);
    }
    if(ino != NULL){
        if(ino->type == 3){
            char s[100];
            int n = readinode(ino, s, 0, ino->size);
            s[ino->size] = 0;
            //printf("s: %s\n", s);
            ls(s);
            return;
        }
        if(ino->type != 1){
            printf("not a dir\n");
        }
        else{
            dirent de;
            for(int off = 0; off < ino->size; off += sizeof(dirent)){
                if(readinode(ino, (char *)&de, off, sizeof(dirent)) != sizeof(dirent)){
                    printf("ls read dirent wrong\n");
                    return;
                }
                if(de.inum != 0){
                    inode *file = inode_number_to_inode(de.inum);
                    char tp[4][10] = {"", "dir", "file", "soft"};
                    printf("Type: %s Name: %s Inode number: %d\n", tp[file->type], de.name, de.inum);
                }
            }
        }
    }else{
        printf("not have this dir\n");
    }
}