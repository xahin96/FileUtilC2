#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variable to store the target filename
char targetFilename[100];

// Function to check if the filename matches the target filename
int isTargetFile(const char *filename)
{
    return strcmp(filename, targetFilename) == 0;
}

// Function to display information about the file or directory
int display_info(
    const char *fpath,
    const struct stat *sb,
    int tflag,
    struct FTW *ftwbuf)
{
    printf("%s  |  %d  |  %s \n", fpath, ftwbuf->base, fpath + ftwbuf->base);
    if (isTargetFile(fpath + ftwbuf->base))
    {
        printf("File '%s' found at: %s\n", targetFilename, fpath);
        exit(EXIT_SUCCESS);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <path> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Set the target filename
    strncpy(targetFilename, argv[2], sizeof(targetFilename));

    // Perform the file tree traversal
    if (nftw(argv[1], display_info, 20, FTW_PHYS) == -1)
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    // If the file is not found, print a message
    printf("File '%s' not found in the specified path.\n", targetFilename);

    exit(EXIT_SUCCESS);
}
