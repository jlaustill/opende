// cli/src/categories/effects.c
#include "effects.h"
#include "../backends/picom.h"
#include "../util/output.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    if (strcmp(setting, EFFECTS_SETTING_COMPOSITOR) == 0) {
        if (picom_start() == 0) {
            print_success("Compositor started");
            return 0;
        }
        return 1;
    }

    if (strcmp(setting, EFFECTS_SETTING_SHADOWS) == 0) {
        if (picom_set_shadows(1) == 0) {
            print_success("Shadows enabled");
            return 0;
        }
        print_error("Failed to enable shadows");
        return 1;
    }

    if (strcmp(setting, EFFECTS_SETTING_ANIMATIONS) == 0) {
        if (picom_set_animations(1) == 0) {
            print_success("Animations enabled");
            return 0;
        }
        print_error("Failed to enable animations");
        return 1;
    }

    if (strcmp(setting, EFFECTS_SETTING_TRANSPARENCY) == 0) {
        if (picom_set_transparency(90) == 0) {
            print_success("Transparency enabled (90%%)");
            return 0;
        }
        print_error("Failed to enable transparency");
        return 1;
    }

    return 1;
}

int effects_disable(const char *setting) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        effects_list_settings();
        return 2;
    }

    if (strcmp(setting, EFFECTS_SETTING_COMPOSITOR) == 0) {
        if (picom_stop() == 0) {
            print_success("Compositor stopped");
            return 0;
        }
        return 1;
    }

    if (strcmp(setting, EFFECTS_SETTING_SHADOWS) == 0) {
        if (picom_set_shadows(0) == 0) {
            print_success("Shadows disabled");
            return 0;
        }
        print_error("Failed to disable shadows");
        return 1;
    }

    if (strcmp(setting, EFFECTS_SETTING_ANIMATIONS) == 0) {
        if (picom_set_animations(0) == 0) {
            print_success("Animations disabled");
            return 0;
        }
        print_error("Failed to disable animations");
        return 1;
    }

    if (strcmp(setting, EFFECTS_SETTING_TRANSPARENCY) == 0) {
        if (picom_set_transparency(100) == 0) {
            print_success("Transparency disabled (100%% opacity)");
            return 0;
        }
        print_error("Failed to disable transparency");
        return 1;
    }

    return 1;
}

int effects_set(const char *setting, const char *value) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        effects_list_settings();
        return 2;
    }

    if (strcmp(setting, EFFECTS_SETTING_TRANSPARENCY) == 0) {
        int percent = atoi(value);
        if (percent < 0 || percent > 100) {
            print_error("Transparency must be 0-100");
            return 1;
        }
        if (picom_set_transparency(percent) == 0) {
            print_success("Transparency set to %d%%", percent);
            return 0;
        }
        print_error("Failed to set transparency");
        return 1;
    }

    print_error("Setting '%s' does not support 'set', use enable/disable", setting);
    return 1;
}

int effects_status(const char *setting) {
    if (setting) {
        if (!is_valid_setting(setting)) {
            print_error("Unknown setting '%s'", setting);
            effects_list_settings();
            return 2;
        }

        if (strcmp(setting, EFFECTS_SETTING_COMPOSITOR) == 0) {
            printf("%s\n", picom_is_running() ? "running" : "stopped");
            return 0;
        }
        if (strcmp(setting, EFFECTS_SETTING_SHADOWS) == 0) {
            int val = picom_get_shadows();
            printf("%s\n", val == 1 ? "enabled" : val == 0 ? "disabled" : "unknown");
            return 0;
        }
        if (strcmp(setting, EFFECTS_SETTING_ANIMATIONS) == 0) {
            int val = picom_get_animations();
            printf("%s\n", val == 1 ? "enabled" : val == 0 ? "disabled" : "unknown");
            return 0;
        }
        if (strcmp(setting, EFFECTS_SETTING_TRANSPARENCY) == 0) {
            int val = picom_get_transparency();
            if (val >= 0) printf("%d%%\n", val);
            else printf("unknown\n");
            return 0;
        }
    }

    // Show all effects status
    print_header("Effects (user-level)");

    int compositor = picom_is_running();
    int shadows = picom_get_shadows();
    int animations = picom_get_animations();
    int transparency = picom_get_transparency();

    printf("  Compositor:   %s\n", compositor ? "running" : "stopped");
    printf("  Shadows:      %s\n", shadows == 1 ? "enabled" : shadows == 0 ? "disabled" : "unknown");
    printf("  Animations:   %s\n", animations == 1 ? "enabled" : animations == 0 ? "disabled" : "unknown");
    printf("  Transparency: %s\n", transparency >= 0 ?
           (transparency == 100 ? "disabled (100%)" : "enabled") : "unknown");
    if (transparency >= 0 && transparency < 100) {
        printf("                %d%% inactive window opacity\n", transparency);
    }

    return 0;
}
