#include <dirent.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s directory", argv[0]);
        return 1;
    }

    DIR *dp = opendir(argv[1]);
    if (dp == NULL) {
        perror("opendir");
        return 1;
    }

    printf("Directory %s entries\n", argv[1]);

    struct dirent *dir_entry;
    while ((dir_entry = readdir(dp)) != NULL) {
        printf("  %s\n", dir_entry->d_name);
    }

    closedir(dp);
    return 0;
}