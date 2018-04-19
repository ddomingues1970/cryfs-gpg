#pragma once

#define _GNU_SOURCE //cause stdio.h to include asprintf

#include <sys/types.h> //mode_t
#include <alloca.h> //alloca
#include <libgen.h> //dirname
#include <mntent.h> //hasmntopt, struc mntent
#include <sys/stat.h> //stat, mkdir
#include <errno.h> //errono
#include <string.h> //strcpy, strcat, strdup, strerror, strlen, strcmp
#include <stdio.h>  //fprintf
#include <unistd.h> //truncate, close, read, unlink, write,
#include <stdlib.h> //mkstemp, exit, free getenv, putenv, malloc,

#define MAX_ARGUMENT_COUNT 30

typedef struct { int argc; char  *argv[MAX_ARGUMENT_COUNT]; } argument_s;

char *add_quote(char *str);
int exec_command(argument_s arg);
int checkdir(const char *basedir);
int checkfile(const char *file);
int makedir(const char *pathname, mode_t mode);
int findmnt(const char *mnt);
void truncatefile(const char *filepath, int length);
int checkinstalled(const char *);
void set_tempfilename(char *tmpfilename);
const char *remove_trailing_slash(const char *dir);
