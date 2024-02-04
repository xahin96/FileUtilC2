#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

char * fileName;
char * finalFilePath;
char * sourcePath;
char * destinationPath;

int nftwCopySource(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    if (strcmp(filePath + ftwBuffer->base, fileName) == 0)
    {
        sourcePath = strdup(filePath);
        return 1;
    }
    return 0;
}

int nftwCopyDestination(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    if (fileFlags == FTW_D)
    {
        destinationPath = strdup(filePath);
        return 1;
    }
    return 0;
}

int copyFileUtil(char * sourceP, char * destinationP)
{
    int sourceFd, destinationFd, n, file;
    char buffer[4096];


    sourceFd = open(sourceP, O_RDONLY);
    destinationFd = open(destinationP, O_CREAT | O_WRONLY, 0700);

    while (1) {
        file = read(sourceFd, buffer, 4096);
        if (file == -1) {
            printf("Error reading file.\n");
            exit(1);
        }
        n = file;

        if (n == 0)
            break;

        file = write(destinationFd, buffer, n);
        if (file == -1) {
            printf("Error writing to file.\n");
            exit(1);
        }
    }

    close(sourceFd);
    close(destinationFd);
    return 0;
}

int copyFile(char *argv[], char * option)
{
    int opt = 0;
    if (strcmp(option, "-cp") == 0)
        opt = 0;
    else if (strcmp(option, "-mv") == 0)
        opt = 1;
    else
    {
        printf("Accepted options are: -cp (copy file) and -mv (move file)");
    }
    sourcePath = argv[1];
    destinationPath = argv[2];
    if (nftw(sourcePath, nftwCopySource, 20, FTW_PHYS) == -1) {
        printf("Search Unsuccessful 1\n");
    }
    if (nftw(destinationPath, nftwCopyDestination, 30, FTW_PHYS) == -1) {
        printf("Search Unsuccessful 2\n");
    }
    printf("Source: %s \nDestination: %s\nFilename: %s\n", sourcePath, destinationPath, fileName);
    strcat(destinationPath, "/");
    strcat(destinationPath, fileName);
    printf("Destination: %s\n", destinationPath);

    copyFileUtil(sourcePath, destinationPath);
    if (opt == 1)
        remove(sourcePath);
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
    copyFile(argv, moveOrCopy);
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