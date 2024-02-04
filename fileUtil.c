#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

char * fileName;
char * finalFilePath;

int nftwFunc(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
//    printf("%s  |  %d  |  %s \n", filePath, ftwBuffer->base, filePath + ftwBuffer->base);
    if (strcmp(filePath + ftwBuffer->base, fileName) == 0)
    {
        printf("File found at: %s\n", filePath);
        return 1;
    }
    return 0;
}

int zipFile(int argc, char *argv[])
{
    return 0;
}

int searchFile(char *argv[])
{
    char *rootDir = argv[1];
    if (nftw(rootDir, nftwFunc, 20, FTW_PHYS) == -1) {
        printf("Search Unsuccessful");
    }
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

    fileName = argv[2];

    if (argc == 3)
    {
        searchFile(argv);
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