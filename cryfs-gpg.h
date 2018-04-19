#pragma once

#define _XOPEN_SOURCE 700
#define _GNU_SOURCE //cause stdio.h to include asprintf

#include "encryption.h" //encrypt_password
#include "util.h"
#include <fcntl.h> //open close
#include <errno.h> //errono
#include <string.h> //strcpy, strcat, strdup, strerror, strlen
#include <unistd.h> //truncate, close, read, unlink, write,
#include <stdlib.h> //mkstemp, exit, free getenv, putenv, malloc,
#include <stdio.h>  //stderr
#include <stdbool.h>  //bool, true, false

char *basedir;
char *mountdir;
char *passphrase;

//TODO: Need a better solution - Should not be constant.
const char configdir[] = "/CFG";
const char passphrasefile[] = "EF95246C74537DDAFBFC508615B30";
const char configfile[] = "EF95246C74537DDAFBFC508615B31";
//
#define DEFAULT_BLOCKSIZE 32768

static size_t blocksize = (int)DEFAULT_BLOCKSIZE;

const char tempfilename[] = "/tmp/cryfs-gpg.XXXXXX";

bool newconfig = true;
bool foreground = false;
enum cryfsenv {CRYFS_FRONTEND, CRYFS_NO_UPDATE_CHECK};

static const char *cryfs_opts[] = {"foreground", "f", "logfile:", "config:", "c:", "unmount-idle:",\
"b:", "basedir:", "m:", "mountdir:", "allow-filesystem-upgrade", "show-ciphers", "blocksize:", "s:", "help", "h", "version", "r:", NULL};


void get_options(int, char**);

void set_cryfsenv(enum cryfsenv env);
void set_configfilepath(char *configfilepath);

char *get_encryptedconfigfilepath(void);
char *get_encryptedpassphrasefilepath(void);
char *get_configdirpath(void);
char *get_passphrase(void);

bool validate(void);
bool check_mountdir(void);
bool check_basedir(void);
bool check_configfile(void);
bool check_passphrasefile(void);
bool check_configdir(void);
bool check_cryfs(void);
bool check_gnugpg();
bool check_gnugpg2();

void create_configdir(void);
void create_passphrasefile(void);

bool mountcryfs(void);

int decrypt_passphrasefile(char *encryptedpassphrasefilepath);

void encrypt_configfile(char *configfilepath, char *encryptedconfigfilepath);
void usage();
