//
// by msn
//
#ifndef ABSOLUTE_H
#define ABSOLUTE_H
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "pathname.h"
#include "symbolic_link.h"

uint rootdirno = 1;

void makeroot();
void cd(char *path);
void ls(char *path);
#endif //ABSOLUTE_H