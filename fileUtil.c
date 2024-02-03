#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int nftwfunc(const char *filename, const struct stat *statptr,
             int fileflags, struct FTW *pfwt)
{
    return 0;
}

int zipFile(int argc, char *argv[])
{
    return 0;
}

int searchFile(int argc, char *argv[])
{
    char *rootDir = argv[1];
    char *filename = argv[2];
    printf("Argument: %s\n", rootDir);
    printf("Argument: %s\n", filename);

    int fd_limit = 5;
    int flags = FTW_CHDIR | FTW_DEPTH | FTW_MOUNT;
    int ret;
    ret = nftw(rootDir, nftwfunc, fd_limit, flags);

    return 0;
}

int moveOrCopyFile(int argc, char *argv[])
{
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("No argument passed.\n");
        return 0;
    }

    if (argc == 3)
    {
        searchFile(argc, argv);
        return 0;
    }

    if (argc == 5)
    {
        moveOrCopyFile(argc, argv);
        return 0;
    }

    if (argc == 4)
    {
        zipFile(argc, argv);
        return 0;
    }
    printf("Please follow the manual for further assistance: \n");
    printf("./fileutil [ root_dir] filename\n");
    return 0;
}