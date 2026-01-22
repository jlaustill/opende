// cli/src/backends/tint2.c
#define _POSIX_C_SOURCE 200809L
#include "tint2.h"
#include "../util/config.h"
#include "../util/output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TINT2_CONFIG_NAME "tint2rc"

int tint2_is_installed(void) {
    return system("which tint2 > /dev/null 2>&1") == 0;
}

int tint2_is_running(void) {
    return system("pgrep -x tint2 > /dev/null 2>&1") == 0;
}

int tint2_reload(void) {
    if (!tint2_is_running()) return 0;
    return system("killall -SIGUSR1 tint2") == 0 ? 0 : -1;
}

static char *get_config_path(void) {
    // tint2 uses ~/.config/tint2/tint2rc
    const char *home = getenv("HOME");
    if (!home) return NULL;

    size_t len = strlen(home) + strlen("/.config/tint2/tint2rc") + 1;
    char *path = malloc(len);
    if (!path) return NULL;

    snprintf(path, len, "%s/.config/tint2/tint2rc", home);

    if (!config_file_exists(path)) {
        // Try to copy from system location if exists
        config_ensure_dir(path);
        char cmd[512];
        snprintf(cmd, sizeof(cmd),
                 "[ -f /etc/xdg/tint2/tint2rc ] && cp /etc/xdg/tint2/tint2rc '%s'",
                 path);
        int ret = system(cmd);
        (void)ret;  // Best effort, ignore result
    }

    return path;
}

char *tint2_get_position(void) {
    char *config = get_config_path();
    if (!config) return NULL;

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "grep -E '^panel_position\\s*=' '%s' 2>/dev/null | head -1", config);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        free(config);
        return NULL;
    }

    char line[256];
    char *result = NULL;
    if (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "bottom")) {
            result = strdup("bottom");
        } else if (strstr(line, "top")) {
            result = strdup("top");
        }
    }

    pclose(fp);
    free(config);
    return result ? result : strdup("unknown");
}

int tint2_set_position(const char *position) {
    if (strcmp(position, "top") != 0 && strcmp(position, "bottom") != 0) {
        print_error("Position must be 'top' or 'bottom'");
        return -1;
    }

    char *config = get_config_path();
    if (!config) return -1;

    // tint2 panel_position format: "bottom center horizontal"
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "sed -i 's/^panel_position\\s*=.*/panel_position = %s center horizontal/' '%s'",
             position, config);

    int result = system(cmd);
    free(config);

    if (result == 0) tint2_reload();
    return result == 0 ? 0 : -1;
}

int tint2_get_autohide(void) {
    char *config = get_config_path();
    if (!config) return -1;

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "grep -E '^autohide\\s*=' '%s' 2>/dev/null | head -1", config);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        free(config);
        return -1;
    }

    char line[256];
    int result = -1;
    if (fgets(line, sizeof(line), fp)) {
        result = (strstr(line, "1") != NULL) ? 1 : 0;
    }

    pclose(fp);
    free(config);
    return result;
}

int tint2_set_autohide(int enabled) {
    char *config = get_config_path();
    if (!config) return -1;

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "sed -i 's/^autohide\\s*=.*/autohide = %d/' '%s'",
             enabled ? 1 : 0, config);

    int result = system(cmd);
    free(config);

    if (result == 0) tint2_reload();
    return result == 0 ? 0 : -1;
}

int tint2_get_systray(void) {
    char *config = get_config_path();
    if (!config) return -1;

    // Check if systray is in panel_items
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "grep -E '^panel_items\\s*=' '%s' 2>/dev/null | head -1", config);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        free(config);
        return -1;
    }

    char line[256];
    int result = -1;
    if (fgets(line, sizeof(line), fp)) {
        result = (strstr(line, "S") != NULL) ? 1 : 0;  // S = systray
    }

    pclose(fp);
    free(config);
    return result;
}

int tint2_set_systray(int enabled) {
    (void)enabled;  // Currently unused - manual edit required

    char *config = get_config_path();
    if (!config) return -1;

    // This is tricky - need to add/remove 'S' from panel_items
    // For now, just warn that manual edit is needed
    print_warn("Systray toggle requires manual edit of %s", config);
    printf("Look for 'panel_items' line and add/remove 'S'\n");

    free(config);
    return -1;
}
