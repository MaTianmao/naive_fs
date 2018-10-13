//
// by msn
//
#ifndef SYMBOLIC_H
#define SYMBOLIC_H
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "absolute_pathname.h"

void mount(char *path, char *name);
void umount(char *name);

#endif //SYMBOLIC_H