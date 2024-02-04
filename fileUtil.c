#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>

char * fileName;
char * finalFilePath;
char * sourcePath;
char * destinationPath;

int nftwCopySource(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    if (strcmp(filePath + ftwBuffer->base, fileName) == 0)
    {
        sourcePath = filePath;
        return 1;
    }
    return 0;
}

int nftwCopyDestination(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    if (fileFlags == FTW_D)
    {
        destinationPath = filePath;
        return 1;
    }
    return 0;
}

int copyFile(char *argv[])
{
    sourcePath = argv[1];
    destinationPath = argv[2];
    if (nftw(sourcePath, nftwCopySource, 20, FTW_PHYS) == -1) {
        printf("Search Unsuccessful 1\n");
    }
    if (nftw(destinationPath, nftwCopyDestination, 30, FTW_PHYS) == -1) {
        printf("Search Unsuccessful 2\n");
    }
    printf("Source: %s \nDestination: %s\nFilename: %s\n", sourcePath, destinationPath, fileName);
//    strcat(destinationPath, "/");
//    strcat(destinationPath, fileName);
//    printf("Destination: %s\n", destinationPath);

    return 0;
}

int moveFile(char *argv[])
{
    printf("Move File\n");
    return 0;
}

int nftwFunc(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    //    printf("%s  |  %d  |  %s \n", filePath, ftwBuffer->base, filePath + ftwBuffer->base);
    if (strcmp(filePath + ftwBuffer->base, fileName) == 0)
    {
        if (fileFlags == FTW_D)
            printf("Directory found at: %s\n", filePath);
        else if (fileFlags == FTW_F)
            printf("File found at: %s\n", filePath);
        return 1;
    }
    return 0;
}

int zipFile(int argc, char *argv[])
{
    printf("Zip file code\n");
    return 0;
}

int searchFile(char *argv[])
{
    char *rootDir = argv[1];
    fileName = argv[2];
    if (nftw(rootDir, nftwFunc, 20, FTW_PHYS) == -1) {
        printf("Search Unsuccessful 3\n");
    }
    return 0;
}

int moveOrCopyFile(char *argv[])
{
    char * moveOrCopy = argv[3];
    fileName = argv[4];
    if (strcmp(moveOrCopy, "-cp") == 0)
    {
        copyFile(argv);
    }
    else if (strcmp(moveOrCopy, "-cp") == 0)
    {
        moveFile(argv);
    }
    else
    {
        printf("Accepted options are: -cp (copy file) and -mv (move file)");
    }
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
        searchFile(argv);
        return 0;
    }

    if (argc == 5)
    {
        moveOrCopyFile(argv);
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