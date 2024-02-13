#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>

// source file path
char * toFilePath;

// destination file path
char * fromFilePath;

// counting total file with provided extension for tar file
int totalFileNumb = 0;

// initial tar file list with duplicates
char **tarringFileList;

// user provided file name holder
char * providedFileName;

// user provided file extension holder
char * providedFileExten;

// for printing error depending on whether a file is found or not
int fileFoundSuccessfully = 0;

// for resolving and setting the 'fromFilePath' location
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

// for resolving and setting the 'toFilePath' location
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

// Copy file utility method
int copyFileUtil(char * sourceP, char * destinationP)
{
    int readFileFDMV, destinationFd, cpMvFile;
    char cpMvBuffer[4096];

    // opening the file found in the location
    readFileFDMV = open(sourceP, O_RDONLY);

    while (1) {
        // reading the file in location
        cpMvFile = read(readFileFDMV, cpMvBuffer, 4096);
        if (cpMvFile == -1) {
            printf("Search Unsuccessful\n");
            exit(1);
        }

        // opening the destination location
        destinationFd = open(destinationP, O_CREAT | O_WRONLY, 0700);

        // stop if nothing was read
        if (cpMvFile == 0)
            break;

        // writing the file in the destination
        cpMvFile = write(destinationFd, cpMvBuffer, cpMvFile);
        if (cpMvFile == -1) {
            printf("Error writing to cpMvFile.\n");
            exit(1);
        }
    }
    close(readFileFDMV);
    close(destinationFd);
    return 0;
}

// Copy or move file main method
int copyOrMoveFileToLoc(char *passedParamList[], char * option)
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

    // calling NFTW for resolving the from and to location
    if (nftw(fromFilePath, nftwCopySource, 20, FTW_PHYS) == -1) {
        printf("Invalid root_dir\n");
        exit(0);
    }
    if (nftw(toFilePath, nftwCopyDestination, 30, FTW_PHYS) == -1) {
        printf("Search Successful : Invalid storage_dir\n");
        exit(0);
    }
    // generating the full location of the file including name
    strcat(toFilePath, "/");
    strcat(toFilePath, providedFileName);

    // initiating copy file functionality
    copyFileUtil(fromFilePath, toFilePath);
    // run in case of move file
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

// NFTW method for the file search functionality
int mainNFTW(const char *nftwResolvedFileLoc,
             const struct stat *nftwStructStatPointer,
             int fileFlags, struct FTW *nftwStructBfr)
{
    // nftwResolvedFileLoc: the fully qualified path of the file
    // nftwStructBfr->base: an integer location provided by NFTW
    // nftwResolvedFileLoc + nftwStructBfr->base: the file name only with the extension
    // checking if providedFileName matches the traversing file name
    if (strcmp(nftwResolvedFileLoc + nftwStructBfr->base, providedFileName) == 0)
    {
        // setting fileFoundSuccessfully for future output printing
        fileFoundSuccessfully = 1;
        // checking location type
        // FTW_D is directory and FTW_F is file
        if (fileFlags == FTW_D)
            printf("%s\n", nftwResolvedFileLoc);
        else if (fileFlags == FTW_F)
            printf("%s\n", nftwResolvedFileLoc);
    }
    return 0;
}

// method used for counting the total number of files found with
// specific extension
int nftwFuncCount(const char *nftwResolvedFileLoc, const struct stat *nftwStructStatPointer,
             int fileFlags, struct FTW *nftwStructBfr)
{
    if (fileFlags == FTW_F)
    {
        // if the provided extension matches the traversed file extension
        // then the counter is incremented
        const char *fileExtPtr = strrchr(nftwResolvedFileLoc, '.');
        if (fileExtPtr != NULL && strcmp(fileExtPtr, providedFileExten) == 0)
        {
            totalFileNumb++;
        }
    }
    return 0;
}

// Method for storing all the file paths with desired extension
int nftwFuncFileAdd(const char *nftwResolvedFileLoc, const struct stat *nftwStructStatPointer,
                    int fileFlags, struct FTW *nftwStructBfr)
{
    if (fileFlags == FTW_F)
    {
        // getting the traversed file extension
        const char *fileExtPtr = strrchr(nftwResolvedFileLoc, '.');
        // if traversed file extension matches the desired file extension
        // then storing it
        if (fileExtPtr != NULL && strcmp(fileExtPtr, providedFileExten) == 0)
        {
            // dynamically allocating a space size for each file path
            tarringFileList[totalFileNumb] = (char *)malloc(strlen(nftwResolvedFileLoc) + 1);
            // then storing the file path in specific index
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

// search file by extension method
int searchFileByExtension(char * providedFileExtens)
{
    // setting the extension globally for future use
    providedFileExten = providedFileExtens;

    // counting the total number of files with the extension
    if (nftw(fromFilePath, nftwFuncCount, 20, FTW_PHYS) == -1)
    {
        printf("Invalid root_dir\n");
        exit(0);
    }

    // array of array to hold all the files found with the same extension
    // dynamic size allocated by using the file count
    tarringFileList = (char **)malloc(totalFileNumb * sizeof(char *));

    // resetting the total number for future use
    totalFileNumb = 0;
    // using NFTW for generating array of files with desired extensions
    if (nftw(fromFilePath, nftwFuncFileAdd, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
    }
    return 0;
}

// TAR file creation main method
int createTarToLocation(char *passedParamList[])
{
    providedFileExten = passedParamList[3];
    fromFilePath = passedParamList[1];
    toFilePath = passedParamList[2];

    // checking if the destination location exists
    if (nftwDestinationCheckTar(toFilePath) == 1)
    {
        char mkdirCmd[512];
        // if folder doesn't exist then crate the folders
        sprintf(mkdirCmd, "mkdir -p \"%s\" 2>/dev/null", toFilePath);
        int fileCreated = system(mkdirCmd);
        if (fileCreated != 0) {
            printf("Invalid storage_dir. storage_dir creation failed\n");
            exit(0);
        }
    }

    // search for files by provided extensions and generate
    // 'tarringFileList' to store them
    searchFileByExtension(providedFileExten);
    char createTarToDestCMD[512];
    // creating the tar command
    strcpy(createTarToDestCMD, "tar -cf \"");
    strcat(createTarToDestCMD, toFilePath);
    strcat(createTarToDestCMD, "/a1.tar\" --no-recursion \"");
    strcat(createTarToDestCMD, fromFilePath);
    strcat(createTarToDestCMD, "\"");

    // array for keeping a track for duplicate file names
    bool fileAdded[totalFileNumb];
    for (int i = 0; i < totalFileNumb; ++i) {
        fileAdded[i] = false;
    }

    // to hold the unique file added to TAR command
    char finalAddedFiles[totalFileNumb][256];
    int finalFileCount = 0;

    for (int i = 0; i < totalFileNumb; ++i) {
        // If file hasn't been added to the list
        if (!fileAdded[i])
        {
            strcat(createTarToDestCMD, " \"");
            strcat(createTarToDestCMD, tarringFileList[i]);
            strcat(createTarToDestCMD, "\"");

            // Mark this filename as added
            fileAdded[i] = true;

            // Add this filename to the final added files list
            strcpy(finalAddedFiles[finalFileCount++], tarringFileList[i]);

            // storing the file name only to compare
            const char *onlyFileName = strrchr(tarringFileList[i], '/');
            // looping over the whole 'fileAdded' list and making it true for
            // any path that has the same file name
            for (int j = i + 1; j < totalFileNumb; ++j) {
                if (strcmp(strrchr(tarringFileList[j], '/'), onlyFileName) == 0) {
                    fileAdded[j] = true;
                }
            }
        }
    }

    // changing directory to source file path so that the paths used
    // for TAR is relative to source
    chdir(fromFilePath);
    // running the command
    int result = system(createTarToDestCMD);
    if (result == 0) {
        for (int i = 0; i < finalFileCount; ++i) {
            // printing the successfully added files
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
    // calling nftw for checking the location, returns -1 if fails
    if (nftw(srcFolderLoc, mainNFTW, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
        exit(0);
    }
    // fileFoundSuccessfully is 1 if provided file name was found in
    // the provided directory
    if (fileFoundSuccessfully == 0)
    {
        printf("Search Unsuccessful\n");
    }
    return 0;
}

int moveOrCopyFileToLoc(char *passedParamList[])
{
    char * moveOrCopyOption = passedParamList[3];
    providedFileName = passedParamList[4];
    copyOrMoveFileToLoc(passedParamList, moveOrCopyOption);
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

/*
ASP Assignment Commands


        Search File
1. Successful first search
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1 p1.c
2. Unsuccessful file name
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1 b2.c
3. Same file name
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6 a1.c


        Copy/Move File
1. Successful Cop
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1 ~/My\ Files/test/task\ 1\ copy\ 6/folder2 -cp b3.pdf
2. Unsuccessful copy
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1 ~/My\ Files/test/task\ 1\ copy\ 6/folder2 -cp b5.pdf
3. invalid root_dir
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1/folder5 ~/My\ Files/test/task\ 1\ copy\ 6/folder2 -cp b5.pdf
4. Invalid storage_dir
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1 ~/My\ Files/test/task\ 1\ copy\ 6/folder9 -cp b4.pdf
5. Successful move
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1 ~/My\ Files/test/task\ 1\ copy\ 6/folder2 -mv p1.c

        TAR
1. Successful in new created folder
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1 ~/My\ Files/test/task\ 1\ copy\ 6/folder2/folder2222 .c
2. Successful in new recursive folder
./fileutil ~/My\ Files/test/task\ 1\ copy\ 6/folder1 ~/M\ Files/test/task\ 1\ copy\ 6/folder2 .pdf
 */