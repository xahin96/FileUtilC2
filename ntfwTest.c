#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>

char * fileExt2;
char * fileName2;
char * sourcePath2;
char * destinationPath2;

int nftwFunc(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    if (fileFlags == FTW_F)
    {
        const char *fileExtPtr = strrchr(filePath, '.');
        if (fileExtPtr != NULL && strcmp(fileExtPtr + 1, fileExt2) == 0)
        {
            printf("File with extension found at: %s\n", filePath);
            return 1;
        }
    }
    return 0;
}

int searchFile(char *argv[])
{
    char *rootDir = argv[1];
    fileName2 = argv[2];
    if (nftw(rootDir, nftwFunc, 20, FTW_PHYS) == -1) {
        printf("Search Unsuccessful 3\n");
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        searchFile(argv);
        return 0;
    }
    return 0;
}