#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

char * fileExt;
char * fileName;
char * sourcePath;
char * destinationPath;
int fileCount = 0;
int fileFound = 0;
char **filePaths;

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

    while (1) {
        file = read(sourceFd, buffer, 4096);
        if (file == -1) {
            printf("Search Unsuccessful\n");
            exit(1);
        }
        destinationFd = open(destinationP, O_CREAT | O_WRONLY, 0700);

        // TODO: n=?
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
        printf("Invalid root_dir\n");
    }
    if (nftw(destinationPath, nftwCopyDestination, 30, FTW_PHYS) == -1) {
        printf("Search Successful : Invalid storage_dir\n");
    }
    strcat(destinationPath, "/");
    strcat(destinationPath, fileName);

    copyFileUtil(sourcePath, destinationPath);
    if (opt == 1)
    {
        remove(sourcePath);
        printf("Search Successful\n");
        printf("File moved to the storage_dir\n");
        return 0;
    }
    printf("Search Successful\n");
    printf("File copied to the storage_dir\n");
    return 0;
}

int nftwFunc(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    if (strcmp(filePath + ftwBuffer->base, fileName) == 0)
    {
        fileFound = 1;
        if (fileFlags == FTW_D)
            printf("%s\n", filePath);
        else if (fileFlags == FTW_F)
            printf("%s\n", filePath);
    }
    return 0;
}

int nftwFuncCount(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    if (fileFlags == FTW_F)
    {
        const char *fileExtPtr = strrchr(filePath, '.');
        if (fileExtPtr != NULL && strcmp(fileExtPtr, fileExt) == 0)
        {
            printf("File with extension found at: %s\n", filePath);
            fileCount++;
        }
    }
    return 0;
}

int nftwFuncFileAdd(const char *filePath, const struct stat *statPtr,
                    int fileFlags, struct FTW *ftwBuffer)
{
    if (fileFlags == FTW_F)
    {
        const char *fileExtPtr = strrchr(filePath, '.');
        if (fileExtPtr != NULL && strcmp(fileExtPtr, fileExt) == 0)
        {
            printf("File with extension found at: %s\n", filePath);

            filePaths[fileCount] = (char *)malloc(strlen(filePath) + 1);
            strcpy(filePaths[fileCount], filePath);

            fileCount++;
        }
    }
    return 0;
}

int searchFileByExtension(char * fileExtension)
{
    fileExt = fileExtension;

    if (nftw(sourcePath, nftwFuncCount, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
    }

    printf("Total files with extension %s found: %d\n", fileExt, fileCount);

    filePaths = (char **)malloc(fileCount * sizeof(char *));

    fileCount = 0;
    if (nftw(sourcePath, nftwFuncFileAdd, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
    }
    return 0;
}

int zipFile(char *argv[])
{
    fileExt = argv[3];
    sourcePath = argv[1];
    destinationPath = argv[2];
    searchFileByExtension(fileExt);
    char tarCmd[512];
    strcpy(tarCmd, "tar -cf \"");
    strcat(tarCmd, destinationPath);
    strcat(tarCmd, "/a1.tar\" --no-recursion \"");
    strcat(tarCmd, sourcePath);
    strcat(tarCmd, "\"");

    for (int i = 0; i < fileCount; ++i) {
        strcat(tarCmd, " \"");
        strcat(tarCmd, filePaths[i]);
        strcat(tarCmd, "\"");
    }

    chdir(sourcePath);
    int result = system(tarCmd);
    printf("%s\n", tarCmd);
    if (result == 0) {
        printf("Tar file created successfully\n");
    } else {
        printf("Error creating tar file\n");
    }

    for (int i = 0; i < fileCount; i++)
    {
        free(filePaths[i]);
    }
    free(filePaths);
    return 0;
}

int searchFile(char *argv[])
{
    char *rootDir = argv[1];
    fileName = argv[2];
    if (nftw(rootDir, nftwFunc, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
        exit(0);
    }
    if (fileFound == 0)
    {
        printf("Search Unsuccessful\n");
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

int printSuggestion()
{
    printf("Please follow the manual for further assistance: \n");
    printf("[1] For seaching a file:\n");
    printf("./fileutil [root_dir] filename\n");
    printf("[2] For moving or copying a file:\n");
    printf("./fileutil [root_dir] [storage_dir] [options] filename\n");
    printf("[3] For zipping specific types of files to a folder:\n");
    printf("./fileutil [root_dir] [storage_dir] extension \n\n");
    return 0;
}

int printWelcome()
{
    printf("         _______  __   __       _______  __    __  .___________. __   __      \n");
    printf("        |   ____||  | |  |     |   ____||  |  |  | |           ||  | |  |     \n");
    printf("        |  |__   |  | |  |     |  |__   |  |  |  | `---|  |----`|  | |  |     \n");
    printf("        |   __|  |  | |  |     |   __|  |  |  |  |     |  |     |  | |  |     \n");
    printf("        |  |     |  | |  `----.|  |____ |  `--'  |     |  |     |  | |  `----.\n");
    printf("        |__|     |__| |_______||_______| \\______/      |__|     |__| |_______|\n");
    return 0;
}

int emptyVars()
{
    if (sourcePath != NULL) {
        free(sourcePath);
    }
    if (destinationPath != NULL) {
        free(destinationPath);
    }
    return 0;
}

int main(int argc, char *argv[])
{
//    printWelcome();

    if (argc <= 2) {
        printSuggestion();
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
        printf("argc <= 4\n");
        zipFile(argv);
        return 0;
    }
    printSuggestion();
    return 0;
}