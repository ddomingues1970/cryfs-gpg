#include "util.h"

int checkdir(const char *dir)
{
    
    struct stat sb;
    
    stat( dir, &sb );

    return S_ISDIR(sb.st_mode) ? EEXIST : ENOENT;
        
}

int checkfile(const char *file)
{
    
    struct stat sb;
    
    stat( file, &sb );

    return S_ISREG(sb.st_mode) ? EEXIST : ENOENT;

}

int makedir(const char *pathname, mode_t mode){

        int ret = mkdir(pathname, mode);
    
        if (  ret < 0 )
        {

        	fprintf(stderr, "%s: %s\n", pathname, strerror(ret));

        }
        
        return ret;

}

int findmnt(const char *mountdir)
{
    
    const char filename[]="/etc/mtab";

    const char mode[]="r"; 
            
    struct _IO_FILE *file = setmntent(filename, mode);
 
    struct mntent *mnt;
            
    while ((mnt = getmntent(file)) != NULL)
    {

        #if 0
            printf("%s %s %s %s %d %d\n", mnt->mnt_fsname, mnt->mnt_dir, mnt->mnt_type, mnt->mnt_opts, mnt->mnt_freq, mnt->mnt_passno);
        #endif


    	if ( strcmp(mnt->mnt_dir, mountdir) == 0)
        {

        	endmntent(file);

        	return (EBUSY); // Device or resource busy. (Already mounted)

        }
        
    }
        
    endmntent(file);

    return (0);
    
}

void truncatefile(const char *filepath, int length)
{

	if(truncate(filepath, length) < 0)
	{

		fprintf(stderr, "%s:%s", filepath, strerror(errno));

		errno = 0;

	}
}

void set_tempfilename(char *tmpfilename)
{

	int fd = mkstemp(tmpfilename);

	if (fd < 0)
	{

			fprintf(stderr, "%s:%s", tmpfilename, strerror(errno));

			errno = 0;

	}

	unlink(tmpfilename);

	close(fd);

}

const char *remove_trailing_slash(const char *dir)
{

	char slashX[] = "/X";

	char *dirpath;

	asprintf(&dirpath, "%s%s", dir, slashX);

	return dirname(dirpath);

}

int exec_command(argument_s arg)
{

	char *cmdline = NULL;

	for (size_t i = 0; i < arg.argc; ++i)
	{

		(cmdline == NULL) ? asprintf(&cmdline, "%s", arg.argv[i]) :	asprintf(&cmdline, "%s %s", cmdline, arg.argv[i]);

		arg.argv[i] = NULL;

		free(arg.argv[i]);

	}

	int ret = system(cmdline); //TODO: Check return codes.

	cmdline = NULL;

	free(cmdline);

	return ret;

}

char *add_quote(char *str)
{

	char *string;

	asprintf(&string, "\'%s\'", str);

	return string;

}
