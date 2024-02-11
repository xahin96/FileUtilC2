#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>

char * providedFileExten;
char * providedFileName;
char * fromFilePath;
char * toFilePath;
int totalFileNumb = 0;
int fileFoundSuccessfully = 0;
char **tarringFileList;

int nftwCopySource(const char *nftwResolvedFileLoc, const struct stat *nftwStructStatPointer,
             int fileFlags, struct FTW *nftwStructBfr)
{
    if (strcmp(nftwResolvedFileLoc + nftwStructBfr->base, providedFileName) == 0)
    {
        fromFilePath = strdup(nftwResolvedFileLoc);
        return 1;
    }
    return 0;
}

int nftwCopyDestination(const char *nftwResolvedFileLoc, const struct stat *nftwStructStatPointer,
             int fileFlags, struct FTW *nftwStructBfr)
{
    if (fileFlags == FTW_D)
    {
        toFilePath = strdup(nftwResolvedFileLoc);
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

int copyFile(char *passedParamList[], char * option)
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
    fromFilePath = passedParamList[1];
    toFilePath = passedParamList[2];
    if (nftw(fromFilePath, nftwCopySource, 20, FTW_PHYS) == -1) {
        printf("Invalid root_dir\n");
    }
    if (nftw(toFilePath, nftwCopyDestination, 30, FTW_PHYS) == -1) {
        printf("Search Successful : Invalid storage_dir\n");
    }
    strcat(toFilePath, "/");
    strcat(toFilePath, providedFileName);

    copyFileUtil(fromFilePath, toFilePath);
    if (opt == 1)
    {
        remove(fromFilePath);
        printf("Search Successful\n");
        printf("File moved to the storage_dir\n");
        return 0;
    }
    printf("Search Successful\n");
    printf("File copied to the storage_dir\n");
    return 0;
}

int mainNFTW(const char *nftwResolvedFileLoc, const struct stat *nftwStructStatPointer,
             int fileFlags, struct FTW *nftwStructBfr)
{
    if (strcmp(nftwResolvedFileLoc + nftwStructBfr->base, providedFileName) == 0)
    {
        fileFoundSuccessfully = 1;
        if (fileFlags == FTW_D)
            printf("%s\n", nftwResolvedFileLoc);
        else if (fileFlags == FTW_F)
            printf("%s\n", nftwResolvedFileLoc);
    }
    return 0;
}

int nftwFuncCount(const char *nftwResolvedFileLoc, const struct stat *nftwStructStatPointer,
             int fileFlags, struct FTW *nftwStructBfr)
{
    if (fileFlags == FTW_F)
    {
        const char *fileExtPtr = strrchr(nftwResolvedFileLoc, '.');
        if (fileExtPtr != NULL && strcmp(fileExtPtr, providedFileExten) == 0)
        {
            totalFileNumb++;
        }
    }
    return 0;
}

int nftwFuncFileAdd(const char *nftwResolvedFileLoc, const struct stat *nftwStructStatPointer,
                    int fileFlags, struct FTW *nftwStructBfr)
{
    if (fileFlags == FTW_F)
    {
        const char *fileExtPtr = strrchr(nftwResolvedFileLoc, '.');
        if (fileExtPtr != NULL && strcmp(fileExtPtr, providedFileExten) == 0)
        {
            tarringFileList[totalFileNumb] = (char *)malloc(strlen(nftwResolvedFileLoc) + 1);
            strcpy(tarringFileList[totalFileNumb], nftwResolvedFileLoc);

            totalFileNumb++;
        }
    }
    return 0;
}

int nftwDestinationCheckTar(char *destinationLocation)
{
    if (nftw(destinationLocation, nftwCopyDestination, 30, FTW_PHYS) == -1) {
        return 1;
    }
    return 0;
}

int searchFileByExtension(char * providedFileExtens)
{
    providedFileExten = providedFileExtens;

    if (nftw(fromFilePath, nftwFuncCount, 20, FTW_PHYS) == -1)
    {
        printf("Invalid root_dir\n");
        exit(0);
    }

    tarringFileList = (char **)malloc(totalFileNumb * sizeof(char *));

    totalFileNumb = 0;
    if (nftw(fromFilePath, nftwFuncFileAdd, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
    }
    return 0;
}

int createTarToLocation(char *passedParamList[])
{
    providedFileExten = passedParamList[3];
    fromFilePath = passedParamList[1];
    toFilePath = passedParamList[2];
    if (nftwDestinationCheckTar(toFilePath) == 1)
    {
        char mkdirCmd[512];
        sprintf(mkdirCmd, "mkdir \"%s\" 2>/dev/null", toFilePath);
        int fileCreated = system(mkdirCmd);
        if (fileCreated != 0) {
            printf("Invalid storage_dir. storage_dir creation failed\n");
            exit(0);
        }
    }
    else
    {
        printf("Invalid storage_dir. storage_dir creation failed\n");
        exit(0);
    }
    searchFileByExtension(providedFileExten);
    char tarCmd[512];
    strcpy(tarCmd, "tar -cf \"");
    strcat(tarCmd, toFilePath);
    strcat(tarCmd, "/a1.tar\" --no-recursion \"");
    strcat(tarCmd, fromFilePath);
    strcat(tarCmd, "\"");

    bool fileAdded[totalFileNumb];
    for (int i = 0; i < totalFileNumb; ++i) {
        fileAdded[i] = false;
    }

    char finalAddedFiles[totalFileNumb][256];
    int finalFileCount = 0;

    for (int i = 0; i < totalFileNumb; ++i) {
        if (!fileAdded[i]) { // If file hasn't been added yet
            strcat(tarCmd, " \"");
            strcat(tarCmd, tarringFileList[i]);
            strcat(tarCmd, "\"");

            // Mark this filename as added
            fileAdded[i] = true;

            // Add this filename to the final added files list
            strcpy(finalAddedFiles[finalFileCount++], tarringFileList[i]);

            // Check and mark other files with the same name
            const char *onlyFileName = strrchr(tarringFileList[i], '/');
            for (int j = i + 1; j < totalFileNumb; ++j) {
                if (strcmp(strrchr(tarringFileList[j], '/'), onlyFileName) == 0) {
                    fileAdded[j] = true;
                }
            }
        }
    }

    chdir(fromFilePath);
    int result = system(tarCmd);
    if (result == 0) {
        for (int i = 0; i < finalFileCount; ++i) {
            printf("%s\n", finalAddedFiles[i]);
        }
        if (totalFileNumb == 0)
            printf("No file found with provided extension\n");
    } else {
        printf("Error creating tar file\n");
    }

    for (int i = 0; i < totalFileNumb; i++)
    {
        free(tarringFileList[i]);
    }
    free(tarringFileList);
    return 0;
}

int fileFinderInLoc(char *passedParamList[])
{
    char *srcFolderLoc = passedParamList[1];
    providedFileName = passedParamList[2];
    if (nftw(srcFolderLoc, mainNFTW, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
        exit(0);
    }
    if (fileFoundSuccessfully == 0)
    {
        printf("Search Unsuccessful\n");
    }
    return 0;
}

int moveOrCopyFileToLoc(char *passedParamList[])
{
    char * moveOrCopy = passedParamList[3];
    providedFileName = passedParamList[4];
    copyFile(passedParamList, moveOrCopy);
    return 0;
}

int printSuggestionToUser()
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

int main(int totalParamsPassed, char *passedParamList[])
{
    // for file searching functionalities
    if (totalParamsPassed == 3)
    {
        fileFinderInLoc(passedParamList);
        return 0;
    }
    // for moving/copying files
    if (totalParamsPassed == 5)
    {
        moveOrCopyFileToLoc(passedParamList);
        return 0;
    }
    // for zipping files
    if (totalParamsPassed == 4)
    {
        createTarToLocation(passedParamList);
        return 0;
    }
    // if nothing matches then printing suggestions to user
    printSuggestionToUser();
    return 0;
}