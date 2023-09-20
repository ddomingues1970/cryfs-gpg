/*********************************************************
 * cryfs-gpg:  mount a CryFS files system.               *
 *                                                       *
 * Purpose:                                              *
 *                                                       *
 * Encrypt and decrypt the CryFS config and password     *
 * files using PGP encryption (GnuPG).         *         *
 * Both files are renamed to the CryFS file standard     *
 * name format and are truncate to the same block size.  *
 * So they  are disguised among the other CryFS files.   *
 * Password is generate using crypt SHA512 -             *
 * 86 characters + salt (user name)                      *
 * The password file is encrypted using PGP public key.  *
 *                                                       *
 * Benefits:                                             *
 * You don't need to remember or keep the CryFS password.*
 * Up to 15 public keys can be informed, so you can      *
 * share the encrypted file system without sharing your  *
 * password.                                             *
 * You can change your PGP password as many times you    *
 * want without effecting the CryFS password.            *
 * You don't need inform the CryFS config when mounting  *                                                       *
 * the file system. The file is decrypted on the fly.    *
 *                                                       *
 * Compile linking with -lcrypt                           *
 *                                                       *
 * Author: Daniel Domingues                              *
 * ddomingues@gmx.com                                    *                                   *
 *                                                       *
 *                                                       *
 * Usage:                                                *
 *                                                       *
 * mountcryfs -b basedir -m mountdir [-r pgp publickey ] *                              *
 *                                                       *
 * version 0.9.9                                         *
 *********************************************************/

#include "cryfs-gpg.h"

int main(int argc, char** argv)
{

	get_options(argc, argv);


	if ( validate() == false )
	{

		return EXIT_FAILURE;

	}


	if( check_configdir() == false )
	{

		create_configdir();

	}
	else
	{

		newconfig = !check_configfile();

	}


	if( check_passphrasefile() == false )
	{

		create_passphrasefile();

	}

	return ( mountcryfs() ) ? EXIT_SUCCESS : EXIT_FAILURE;

}

bool validate()
{


	return (  check_basedir()  && \
			  check_mountdir() && \
			  check_cryfs() && \
			  check_gnugpg2() \
		   ) ? true : false;

}

void get_options(int argc, char** argv)
{

	if (argc < 2)
	{

		usage();

		exit (EXIT_FAILURE);

	}

    char *optstring = NULL;

    for (int i = 0; cryfs_opts[i] != NULL ; ++i)
    {

    	if(strlen(cryfs_opts[i]) <= 2) //Only one letter options
    	{

    		(optstring == NULL) ? asprintf(&optstring, "%s", cryfs_opts[i] ) :	asprintf(&optstring, "%s%s", optstring, cryfs_opts[i]);

    	}

    }

    int opt, r_count=0;

	while ((opt = getopt(argc, argv, optstring)) != -1)
	{

		switch (opt)
		{
			case 'b': 	basedir = strdup(remove_trailing_slash(optarg));
						break;

			case 'm': 	mountdir = strdup(remove_trailing_slash(optarg));
						break;

			case 'r': 	if(r_count > MAX_RECIPIENT_COUNT)
						{
							fprintf(stderr, "Too many recipients.\n");
							break;
						}
						asprintf(&gpg_recipient[r_count], "%s", optarg);
						++r_count;
						break;

			case 's':   blocksize = atoi(optarg);
						break;
						//TODO: Implement Additional options

			case 'f':	foreground = true;
						break;

			case 'c':
						break;

			case 'h':	usage();
						exit (EXIT_SUCCESS);

			default: 	fprintf(stderr, "Invalid argument\n");
			         	usage();
			         	exit (EXIT_FAILURE);

		}

	}

	free(optstring);

}

char *get_encryptedpassphrasefilepath()
{

	char *encryptedpassphrasfilepath;

    asprintf(&encryptedpassphrasfilepath, "%s%s/%s", basedir, configdir, passphrasefile);

    return encryptedpassphrasfilepath;

}

char  *get_encryptedconfigfilepath()
{

    char *encryptedconfigfilepath;

    asprintf(&encryptedconfigfilepath, "%s%s/%s", basedir, configdir, configfile);

    return encryptedconfigfilepath;

}

void set_configfilepath(char *configfilepath)
{

	set_tempfilename(configfilepath);

	int ret = decrypt_file(get_encryptedconfigfilepath(), configfilepath);

    if ( ret != 0 && ret != 512)
    {

    	exit(ret);

    }

}

bool mountcryfs()
{

    char *configfilepath = strdup(tempfilename);

    if ( newconfig )
    {

    	set_tempfilename(configfilepath);

    	set_cryfsenv(CRYFS_FRONTEND);

    }
    else
    {

    	set_configfilepath(configfilepath);

    }

    if (passphrase == NULL)
    {

    	passphrase = add_quote(get_passphrase());

    }

    argument_s arg = {};

    size_t index = 0;

    asprintf( &arg.argv[index], "echo -n"); ++index;
    asprintf( &arg.argv[index], "%s", passphrase); ++index;
    asprintf( &arg.argv[index], " | cryfs --config"); ++index;
    asprintf( &arg.argv[index], "%s", configfilepath); ++index;
    if (blocksize != (size_t)DEFAULT_BLOCKSIZE)
	{
		asprintf( &arg.argv[index], "--blocksize %lu", blocksize); ++index;
	}
    if (foreground)
    {
    	asprintf( &arg.argv[index], "--foreground"); ++index;
    }
	//TODO: Add additional cryfs arguments
    asprintf( &arg.argv[index], "%s", basedir); ++index;
    asprintf( &arg.argv[index], "%s", mountdir); ++index;
	//TODO: Add additional cryfs arguments


    arg.argc = index;

	int ret = exec_command(arg);

	(newconfig) ? encrypt_configfile(configfilepath, get_encryptedconfigfilepath()) : remove(configfilepath);

    if ( ret != 0 && !(ret == 2 && foreground) ) //ret = 2 Control-c was pressed, when using foreground option
    {

    	fprintf(stderr, "%s: error code=%ld\n", "cryfs", (long)ret);

    	return false;

    }


	return true;

}

bool check_basedir()
{
    
    if (checkdir(basedir) == ENOENT)
    { //No such file or directory.

        fprintf(stderr, "%s: %s\n", basedir, strerror(ENOENT));

        usage();

        return false;

    }   

    return true;
}

bool check_configfile()
{

   return (checkfile(get_encryptedconfigfilepath()) == EEXIST) ? true : false;

}

char *get_configdirpath()
{

    char *configdirpath;

    asprintf(&configdirpath, "%s%s", basedir, configdir);

    return configdirpath;

}

bool check_passphrasefile()
{

   return ( checkfile( get_encryptedconfigfilepath() ) == EEXIST) ? true : false;

}

bool check_configdir()
{

    return ( checkdir( get_configdirpath() ) == EEXIST ) ? true : false;

}

void create_configdir()
{

       if ( makedir(get_configdirpath(), S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH ) != 0)  //u+rwx,g+rwx,o+r
       {

    	   fprintf(stderr, "%s:%s", get_configdirpath(), strerror(errno) );

    	   exit (errno);

       }

}


//Check if mountdir exists and if it's already mounted. 
bool check_mountdir()
{

	if ( checkdir(mountdir) == ENOENT )
	{ //No such file  or directory.

		fprintf(stderr, "%s: %s\n", mountdir, strerror(ENOENT));

		return false;

	}

    if( findmnt(mountdir) == EBUSY )
    { //Device or resource busy (Directory already mounted)

    	fprintf(stderr, "%s: %s\n", mountdir, strerror(EBUSY));

        return false;

    }

    return true;
}

char *get_passphrase()
{

    size_t max_buffer_size = 1024;

    char *password = (char*)malloc( max_buffer_size );

    if (password == NULL)
    {
    	fprintf(stderr, "get_passphrase(): %s\n", strerror(errno));

    	exit (errno);

    }

    if ( checkfile(get_encryptedpassphrasefilepath()) == ENOENT ) //No such file or directory.
	{

    	fprintf(stderr, "%s: %s\n", get_encryptedpassphrasefilepath(), strerror(ENOENT));

    	exit (ENOENT);

	}

    int fd_passphrasefilepath = decrypt_passphrasefile(get_encryptedpassphrasefilepath());

    if ( read(fd_passphrasefilepath, password, max_buffer_size ) < 0 )
    {

    	int ret = errno;

    	fprintf(stderr, "%s\n", strerror(ret));

        if ( close(fd_passphrasefilepath) < 0 )
        {

        	fprintf(stderr, "%s\n", strerror(errno));

        }

        exit (ret);

    }

    if ( close(fd_passphrasefilepath) < 0 )
    {

    	fprintf(stderr, "%s\n", strerror(errno));

    }

    return password;

}

void create_passphrasefile()
{
	char *passphrasefilepath = strdup(tempfilename);

	set_tempfilename(passphrasefilepath);

	//Using the unique generated temporary file name as the password to be encrypted
	char *password_to_encrypt = strdup(passphrasefilepath);
	//Using the user name as salt to encrypt the password
	char *salt =  strdup(getenv( "USER" ));

    enum hash mode = sha512;

    char *password = strdup(encrypt_password(password_to_encrypt, salt, mode));

    int fd_passphrasefilepath = open(passphrasefilepath, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, S_IRWXU); //u+rwx

    if (fd_passphrasefilepath < 0)
    {

        fprintf(stderr, "%s: %s", passphrasefilepath, strerror(errno));

        exit(errno);

    }

    if ( write(fd_passphrasefilepath, password, strlen(password)+1) < 0)
    {

        fprintf(stderr, "%s: %s", passphrasefilepath, strerror(errno));

        exit(errno);

    }

    int ret = encrypt_file(passphrasefilepath, get_encryptedpassphrasefilepath());

    if ( ret != 0 )
    {

    	exit(errno);

    }

    truncatefile(get_encryptedpassphrasefilepath(), blocksize);

    if ( close(fd_passphrasefilepath) < 0 )
    {

    	fprintf(stderr, "%s: %s\n", passphrasefilepath, strerror(errno));

    }

    if ( remove(passphrasefilepath) < 0 )
    {

    	fprintf(stderr, "%s: %s\n", passphrasefilepath, strerror(errno));

    }

    passphrase = add_quote(password);

}


bool check_gnugpg()
{
    
	argument_s arg = {};

	arg.argc = 1;

	asprintf(&arg.argv[0], "gpg --version 2>&1 > /dev/null");

	int ret = exec_command(arg);

	if ( ret != 0 )
	{

		fprintf(stderr, "gpg: Not installed");

	}

	return ( ret == 0 ) ? true : false;

}

bool check_gnugpg2()
{

	argument_s arg = {};

	arg.argc = 1;

	asprintf(&arg.argv[0], "gpg --version 2>&1 > /dev/null");

	int ret = exec_command(arg);

	if ( ret != 0 )
		fprintf(stderr, "gpg2: Not installed");

	return ( ret == 0 ) ? true : false;

}

bool check_cryfs()
{

	argument_s arg = {};

	arg.argc = 1;

	asprintf(&arg.argv[0], "cryfs --version 2>&1 > /dev/null");

	int ret = exec_command(arg);

    if ( ret != 0 )
    {

    	fprintf(stderr, "cryfs: Not installed.");

    }

	return ( ret == 0 ) ? true : false;
    
}

int decrypt_passphrasefile(char *encryptedpassphrasefilepath)
{

	 char *passphrasefilepath = strdup(tempfilename);

	 int fd_passphrasefile = mkstemp(passphrasefilepath);

	 if (fd_passphrasefile < 0)
	 {

		 fprintf(stderr, "%s: %s", passphrasefilepath, strerror(errno));

		 exit(errno);

	 }

	 int ret = decrypt_file(encryptedpassphrasefilepath, passphrasefilepath);

	 if ( ret != 0 && ret != 512 ) //encrypted message has been manipulated - This occurs because the file size was truncate to 32768
	 {

		 unlink(passphrasefilepath);

		 fprintf(stderr, "Error decrypting passphrasefile ret=%ld\n", (long)ret);

		 exit(ret);

	 }

	 unlink(passphrasefilepath);

	 free(encryptedpassphrasefilepath);

	 return fd_passphrasefile;

}


void encrypt_configfile(char *configfilepath, char *encryptedconfigfilepath)
{

    int ret = encrypt_file(configfilepath, encryptedconfigfilepath);

    if ( remove(configfilepath) < 0 )
    {

    	fprintf(stderr, "%s: %s\n", configfilepath, strerror(errno));

    }

    if (ret  != 0)
    {

    	exit (ret);

    }

    if(truncate(encryptedconfigfilepath, blocksize) < 0)
    {

    	fprintf(stderr, "%s:%s", encryptedconfigfilepath, strerror(errno));

    }

}

void set_cryfsenv(enum cryfsenv env)
{

	int ret = 0;

	switch(env)
	{
		case 0:
			ret = putenv( "CRYFS_FRONTEND=noninteractive");
			break;

		case 1:
			ret = putenv( "CRYFS_NO_UPDATE_CHECK=true");
			break;

		default:
			fprintf(stderr, "%d: Invalid cryfs environment option.\n", env);
			break;
	}

	if ( ret != 0)
	{

		fprintf(stderr, "Error setting environment variable. %s", strerror(errno));

		exit (errno);

	}

}

void usage()
{

	//TODO: Review and check for standartusage message

	printf("\n\nusage: cryfs-gpg -b basedir -m mountdir [-r recipient] [-b blocksize]\n\n");
	printf("-r pgp public key id. (email). Max 15 recipients.\n");
	printf("-b blocksize - default 32768\n\n");
	printf("Example:\n\n");
	printf("cryfs-gpg -b /home/user/bd1 -m /home/user/md1 -r recip1@yahoo.com -r recip2@gmail.com\n");


}
