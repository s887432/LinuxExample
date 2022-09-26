#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct stat mountpoint;
struct stat parent;

/* Get the stat structure of the directory...*/
if stat("/mnt", &mountpoint) == -1) {
    perror("failed to stat mountpoint:");
    exit(EXIT_FAILURE);
}

/* ... and its parent. */
if stat("/mnt/..", &parent) == -1) {
    perror("failed to stat parent:");
    exit(EXIT_FAILURE);
}

/* Compare the st_dev fields in the results: if they are
   equal, then both the directory and its parent belong 
   to the same filesystem, and so the directory is not 
   currently a mount point.
*/
if (mountpoint.st_dev == parent.st_dev) {
    printf("No, there is nothing mounted in that directory.\n");
} else {
    printf("Yes, there is currently a filesystem mounted.\n");
}
