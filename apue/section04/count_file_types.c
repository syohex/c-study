#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

typedef void callback_t(const char *path, const struct stat *st, void *arg);

struct file_statistic {
    int regular_files;
    int block_devices;
    int character_devices;
    int fifos;
    int links;
    int sockets;
    int directories;
};

static void print_statistic(char *dir, struct file_statistic *fs) {
    printf("Statistic under %s\n", dir);
    printf("\tregular files:     %d\n", fs->regular_files);
    printf("\tblock_devices:     %d\n", fs->block_devices);
    printf("\tcharacter devices: %d\n", fs->character_devices);
    printf("\tfifos:             %d\n", fs->fifos);
    printf("\tsymbolic links:    %d\n", fs->links);
    printf("\tsockets:           %d\n", fs->sockets);
    printf("\tdirectories:       %d\n", fs->directories);
}

static void file_callback(const char *path, const struct stat *st, void *arg) {
    struct file_statistic *statistic = arg;
    mode_t mode = st->st_mode;
    if (S_ISREG(mode)) {
        ++statistic->regular_files;
    } else if (S_ISBLK(mode)) {
        ++statistic->block_devices;
    } else if (S_ISCHR(mode)) {
        ++statistic->character_devices;
    } else if (S_ISFIFO(mode)) {
        ++statistic->fifos;
    } else if (S_ISLNK(mode)) {
        ++statistic->links;
    } else if (S_ISSOCK(mode)) {
        ++statistic->sockets;
    } else if (S_ISDIR(mode)) {
        ++statistic->directories;
    }
}

static int dir_walk(char *path, size_t buf_len, callback_t fn, void *arg) {
    struct stat st;

    if (lstat(path, &st) < 0) {
        perror("lstat");
        return -1;
    }

    if (!S_ISDIR(st.st_mode)) {
        fn(path, &st, arg);
        return 0;
    }

    fn(path, &st, arg);

    size_t path_len = strlen(path);
    if (path_len > buf_len / 2) {
        buf_len *= 2;
        path = realloc(path, buf_len);
        if (path == NULL) {
            perror("realloc");
            return -1;
        }
    }

    path[path_len] = '/';
    path[path_len + 1] = '\0';
    size_t path_end = path_len + 1;

    DIR *dp = opendir(path);
    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    struct dirent *ent;
    int ret = 0;
    while ((ent = readdir(dp)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        strcpy(&path[path_end], ent->d_name);

        ret = dir_walk(path, buf_len, fn, arg);
        if (ret != 0) {
            break;
        }
    }

    path[path_end - 1] = '\0';
    closedir(dp);

    return ret;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s directory\n", argv[0]);
        return 1;
    }

    struct file_statistic statistic;
    memset(&statistic, 0, sizeof(statistic));

    char *file_path = malloc(PATH_MAX + 1);
    if (file_path == NULL) {
        perror("malloc");
        return 1;
    }

    size_t len = strlen(argv[1]);
    size_t buf_len = PATH_MAX + 1;
    if (len >= PATH_MAX + 1) {
        buf_len = len * 2;
        file_path = realloc(file_path, buf_len);
        if (file_path == NULL) {
            perror("realloc");
            return 1;
        }
    }

    strcpy(file_path, argv[1]);

    (void)dir_walk(file_path, buf_len, file_callback, &statistic);
    print_statistic(file_path, &statistic);
    free(file_path);
    return 0;
}