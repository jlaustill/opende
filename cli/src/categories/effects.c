// cli/src/categories/effects.c
#include "effects.h"
#include "../util/output.h"
#include <stdio.h>
#include <string.h>

static const char *valid_settings[] = {
    EFFECTS_SETTING_COMPOSITOR,
    EFFECTS_SETTING_SHADOWS,
    EFFECTS_SETTING_TRANSPARENCY,
    EFFECTS_SETTING_ANIMATIONS,
    NULL
};

static int is_valid_setting(const char *setting) {
    for (int i = 0; valid_settings[i]; i++) {
        if (strcmp(setting, valid_settings[i]) == 0) return 1;
    }
    return 0;
}

void effects_list_settings(void) {
    printf("Available effects settings:\n");
    for (int i = 0; valid_settings[i]; i++) {
        printf("  %s\n", valid_settings[i]);
    }
}

int effects_enable(const char *setting) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        effects_list_settings();
        return 2;
    }

    print_info("Enabling %s... (not implemented)", setting);
    return 0;
}

int effects_disable(const char *setting) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        effects_list_settings();
        return 2;
    }

    print_info("Disabling %s... (not implemented)", setting);
    return 0;
}

int effects_set(const char *setting, const char *value) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        effects_list_settings();
        return 2;
    }

    print_info("Setting %s to '%s'... (not implemented)", setting, value);
    return 0;
}

int effects_status(const char *setting) {
    if (setting) {
        if (!is_valid_setting(setting)) {
            print_error("Unknown setting '%s'", setting);
            effects_list_settings();
            return 2;
        }
        printf("%s: (not implemented)\n", setting);
    } else {
        print_header("Effects (user-level)");
        printf("  Compositor:   (not implemented)\n");
        printf("  Shadows:      (not implemented)\n");
        printf("  Transparency: (not implemented)\n");
        printf("  Animations:   (not implemented)\n");
    }
    return 0;
}
