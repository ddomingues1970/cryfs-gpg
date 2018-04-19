#pragma once

#define _XOPEN_SOURCE 700
#define _GNU_SOURCE //cause stdio.h to include asprintf

#include "util.h"
#include <crypt.h> //crypt
#include <errno.h> //errono
#include <string.h> //strcpy, strcat, strdup, strerror, strlen
#include <stdio.h>  //fprintf
#define MAX_RECIPIENT_COUNT 15

enum hash {des=0, md5=1, sha256=5, sha512=6};

char *gpg_recipient[MAX_RECIPIENT_COUNT];

const char *encrypt_password(const char *password, const char *salt, enum hash mode);

int encrypt_file(char *src, char *dest);
int decrypt_file(char *src, char *dest);

char *get_recipients();
