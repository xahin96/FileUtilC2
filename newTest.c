#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

char *fileExt;
char *sourcePath;
char *destinationPath;
char *fileExt;
int fileCount = 0;
char **filePaths;

int nftwFuncFileAdd(const char *filePath, const struct stat *statPtr,
                    int fileFlags, struct FTW *ftwBuffer)
{
    if (fileFlags == FTW_F)
    {
        const char *fileExtPtr = strrchr(filePath, '.');
        if (fileExtPtr != NULL && strcmp(fileExtPtr, fileExt) == 0)
        {
            printf("File with extension found at: %s\n", filePath);

            // Dynamically allocate memory for the file path
            filePaths[fileCount] = (char *)malloc(strlen(filePath) + 1);
            strcpy(filePaths[fileCount], filePath);

            fileCount++;  // Update file count
        }
    }
    return 0;
}

int nftwFuncSearch(const char *filePath, const struct stat *statPtr,
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


int nftwFunc(const char *filePath, const struct stat *statPtr,
             int fileFlags, struct FTW *ftwBuffer)
{
    if (fileFlags == FTW_F)
    {
        // Extracting the file extension from the filePath
        const char *fileExtPtr = strrchr(filePath, '.');
        if (fileExtPtr != NULL && strcmp(fileExtPtr + 1, fileExt) == 0)
        {
            printf("File with extension found at: %s\n", filePath);
            return 0;  // Return 0 to continue traversal
        }
    }
    return 0;
}

int searchFile(char *argv[])
{
    char *rootDir = argv[1];
    fileExt = argv[2];

    // First pass to count the files
    if (nftw(rootDir, nftwFunc, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
    }

    printf("Total files with extension %s found: %d\n", fileExt, fileCount);

    filePaths = (char **)malloc(fileCount * sizeof(char *));

    fileCount = 0;  // Reset file count
    if (nftw(rootDir, nftwFuncFileAdd, 20, FTW_PHYS) == -1)
    {
        printf("Search Unsuccessful\n");
    }

    printf("File Paths:\n");
    for (int i = 0; i < fileCount; i++)
    {
        printf("%s\n", filePaths[i]);
    }

    // Don't forget to free the allocated memory
    for (int i = 0; i < fileCount; i++)
    {
        free(filePaths[i]);
    }
    free(filePaths);

    return 0;
}

int main(int argc, char *argv[])
{
    printf("\n%d\n", argc);
    if (argc == 3)
    {
        searchFile(argv);
        return 0;
    }
    return 0;
}
