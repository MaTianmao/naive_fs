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
#include <stdint.h>
#include "symbolic_link.h"
#define uint uint32_t
__attribute__((weak)) uint rootdirno = 1;

void makeroot();
void cd(char *path);
void ls(char *path);
#endif //ABSOLUTE_H
