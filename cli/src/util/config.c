// cli/src/util/config.c
#define _POSIX_C_SOURCE 200809L
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>

char *config_get_user_path(const char *filename) {
    const char *home = getenv("HOME");
    if (!home) return NULL;

    size_t len = strlen(home) + strlen("/.config/opende/") + strlen(filename) + 1;
    char *path = malloc(len);
    if (!path) return NULL;

    snprintf(path, len, "%s/.config/opende/%s", home, filename);
    return path;
}

char *config_get_system_path(const char *filename) {
    const char *prefix = "/etc/X11/xorg.conf.d/";
    size_t len = strlen(prefix) + strlen(filename) + 1;
    char *path = malloc(len);
    if (!path) return NULL;

    snprintf(path, len, "%s%s", prefix, filename);
    return path;
}

int config_ensure_dir(const char *filepath) {
    char *path_copy = strdup(filepath);
    if (!path_copy) return -1;

    char *dir = dirname(path_copy);

    // Try to create directory (may already exist)
    if (mkdir(dir, 0755) == -1 && errno != EEXIST) {
        // Try creating parent directories
        char *p = dir;
        while (*p) {
            if (*p == '/' && p != dir) {
                *p = '\0';
                mkdir(dir, 0755);
                *p = '/';
            }
            p++;
        }
        if (mkdir(dir, 0755) == -1 && errno != EEXIST) {
            free(path_copy);
            return -1;
        }
    }

    free(path_copy);
    return 0;
}

int config_file_exists(const char *path) {
    return access(path, R_OK) == 0;
}
