#include "encryption.h"

const char *encrypt_password(const char *password, const char *salt, enum hash method)
{
	size_t method_prefix_width = 4;

	char method_id[method_prefix_width];

	    switch(method)
	    {
	        case 1:
	            strcpy(method_id,"$1$");
	            break;
	        case 5:
	            strcpy(method_id,"$5$");
	            break;
	        case 6:
	            strcpy(method_id,"$6$");
	            break;
	        case 0:
	        default:
	            strcpy(method_id,"");
	            break;
	    }

	    char *salt_method;

		asprintf(&salt_method, "%s%s",  method_id, salt);

		return crypt(password, salt_method);

}

int encrypt_file(char *src, char *dest)
{

	int ret = checkfile(src);

	if( ret == ENOENT) {

		fprintf(stderr, "%s:%s", src, strerror(ret));

		return ret;

	}

	argument_s arg = {};

	char *gpg_r =  get_recipients();

	size_t index = 0;

	asprintf( &arg.argv[index], "gpg2 -e"); ++index;
	if (gpg_r != NULL)
	{
		asprintf( &arg.argv[index], "%s", gpg_r); ++index;
	}
	asprintf( &arg.argv[index], "< %s", src); ++index;
	asprintf( &arg.argv[index], "> %s", dest); ++index;

	arg.argc = index;

	ret = exec_command(arg);

    if ( ret != 0 )
    {

    	fprintf(stderr, "%s: Error encrypting to %s ret=%ld\n", src, dest, (long)ret);

    }

    return (ret);

}

int decrypt_file(char *src, char *dest)
{

	int ret = checkfile(src);

	if( ret == ENOENT) {

		fprintf(stderr, "%s:%s", src, strerror(ret));

		return ret;

	}

	argument_s arg = {};

	char *gpg_r =  get_recipients();
	size_t index = 0;

	asprintf( &arg.argv[index], "gpg2 -d"); ++index;
	if (gpg_r != NULL)
	{
		asprintf( &arg.argv[index], "%s", gpg_r); ++index;
	}
	asprintf( &arg.argv[index], "< %s", src); ++index;
	asprintf( &arg.argv[index], "> %s", dest); ++index;
	asprintf( &arg.argv[index], "2>/dev/null"); ++index;

	arg.argc = index;

	ret = exec_command(arg);

	 if ( ret != 0 && ret != 512 ) //encrypted message has been manipulated - This occurs because the file size was truncate to 32768
		 fprintf(stderr, "%s: Error decrypting from %s ret=%ld\n", src, dest, (long)ret);

	 return ret;

}

char *get_recipients()
{

	char *gpg_r = NULL;

	for (size_t i=0; gpg_recipient[i] != NULL; ++i)
	{

		(gpg_r == NULL) ? asprintf(&gpg_r, "-r %s", gpg_recipient[i] ) : asprintf(&gpg_r, "%s -r %s", gpg_r, gpg_recipient[i]);

	}

	return gpg_r;
}
