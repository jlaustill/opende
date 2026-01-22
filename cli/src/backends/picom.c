// cli/src/backends/picom.c
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "picom.h"
#include "../util/config.h"
#include "../util/output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define PICOM_CONFIG_NAME "picom.conf"

int picom_is_installed(void) {
    return system("which picom > /dev/null 2>&1") == 0;
}

int picom_is_running(void) {
    return system("pgrep -x picom > /dev/null 2>&1") == 0;
}

int picom_start(void) {
    if (picom_is_running()) {
        return 0;  // Already running
    }

    if (!picom_is_installed()) {
        print_error("picom is not installed");
        printf("Install with: sudo apt install picom\n");
        return -1;
    }

    // Try user config first, fall back to system
    char *user_config = config_get_user_path(PICOM_CONFIG_NAME);
    const char *config_path = NULL;

    if (user_config && config_file_exists(user_config)) {
        config_path = user_config;
    } else {
        // Use OpenDE system config
        config_path = "/usr/local/share/opende/config/picom.conf";
    }

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "picom -b --config '%s'", config_path);

    int result = system(cmd);
    free(user_config);

    return result == 0 ? 0 : -1;
}

int picom_stop(void) {
    if (!picom_is_running()) {
        return 0;  // Already stopped
    }
    return system("pkill -x picom") == 0 ? 0 : -1;
}

int picom_reload(void) {
    // picom doesn't have a reload signal, so restart
    picom_stop();
    usleep(100000);  // 100ms
    return picom_start();
}

// Config file helpers
static char *get_config_path(void) {
    char *user_config = config_get_user_path(PICOM_CONFIG_NAME);
    if (user_config && config_file_exists(user_config)) {
        return user_config;
    }
    free(user_config);

    // Create user config from system template
    user_config = config_get_user_path(PICOM_CONFIG_NAME);
    if (!user_config) return NULL;

    if (config_ensure_dir(user_config) != 0) {
        free(user_config);
        return NULL;
    }

    // Copy system config to user location
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "cp /usr/local/share/opende/config/picom.conf '%s' 2>/dev/null",
             user_config);
    if (system(cmd) != 0) {
        // File may not exist yet, that's okay
    }

    return user_config;
}

int picom_get_shadows(void) {
    char *config = get_config_path();
    if (!config) return -1;

    // Check if shadow = true/false in config
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "grep -E '^shadow\\s*=' '%s' 2>/dev/null", config);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        free(config);
        return -1;
    }

    char line[256];
    int result = -1;
    if (fgets(line, sizeof(line), fp)) {
        result = (strstr(line, "true") != NULL) ? 1 : 0;
    }

    pclose(fp);
    free(config);
    return result;
}

int picom_set_shadows(int enabled) {
    char *config = get_config_path();
    if (!config) return -1;

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "sed -i 's/^shadow\\s*=.*/shadow = %s/' '%s'",
             enabled ? "true" : "false", config);

    int result = system(cmd);
    free(config);

    if (result == 0 && picom_is_running()) {
        picom_reload();
    }

    return result == 0 ? 0 : -1;
}

int picom_get_animations(void) {
    char *config = get_config_path();
    if (!config) return -1;

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "grep -E '^fading\\s*=' '%s' 2>/dev/null", config);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        free(config);
        return -1;
    }

    char line[256];
    int result = -1;
    if (fgets(line, sizeof(line), fp)) {
        result = (strstr(line, "true") != NULL) ? 1 : 0;
    }

    pclose(fp);
    free(config);
    return result;
}

int picom_set_animations(int enabled) {
    char *config = get_config_path();
    if (!config) return -1;

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "sed -i 's/^fading\\s*=.*/fading = %s/' '%s'",
             enabled ? "true" : "false", config);

    int result = system(cmd);
    free(config);

    if (result == 0 && picom_is_running()) {
        picom_reload();
    }

    return result == 0 ? 0 : -1;
}

int picom_get_transparency(void) {
    char *config = get_config_path();
    if (!config) return -1;

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "grep -E '^inactive-opacity\\s*=' '%s' 2>/dev/null", config);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        free(config);
        return -1;
    }

    char line[256];
    int result = -1;
    if (fgets(line, sizeof(line), fp)) {
        float opacity;
        if (sscanf(line, "inactive-opacity = %f", &opacity) == 1) {
            result = (int)(opacity * 100);
        }
    }

    pclose(fp);
    free(config);
    return result;
}

int picom_set_transparency(int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    char *config = get_config_path();
    if (!config) return -1;

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "sed -i 's/^inactive-opacity\\s*=.*/inactive-opacity = %.2f/' '%s'",
             percent / 100.0, config);

    int result = system(cmd);
    free(config);

    if (result == 0 && picom_is_running()) {
        picom_reload();
    }

    return result == 0 ? 0 : -1;
}
