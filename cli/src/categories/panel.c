// cli/src/categories/panel.c
#include "panel.h"
#include "../backends/tint2.h"
#include "../util/output.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *valid_settings[] = {
    PANEL_SETTING_POSITION,
    PANEL_SETTING_AUTOHIDE,
    PANEL_SETTING_SYSTRAY,
    NULL
};

static int is_valid_setting(const char *setting) {
    for (int i = 0; valid_settings[i]; i++) {
        if (strcmp(setting, valid_settings[i]) == 0) return 1;
    }
    return 0;
}

void panel_list_settings(void) {
    printf("Available panel settings:\n");
    for (int i = 0; valid_settings[i]; i++) {
        printf("  %s\n", valid_settings[i]);
    }
}

int panel_enable(const char *setting) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        panel_list_settings();
        return 2;
    }

    if (strcmp(setting, PANEL_SETTING_AUTOHIDE) == 0) {
        if (tint2_set_autohide(1) == 0) {
            print_success("Autohide enabled");
            return 0;
        }
        print_error("Failed to enable autohide");
        return 1;
    }

    if (strcmp(setting, PANEL_SETTING_SYSTRAY) == 0) {
        if (tint2_set_systray(1) == 0) {
            print_success("Systray enabled");
            return 0;
        }
        return 1;
    }

    print_error("Setting '%s' does not support enable/disable, use 'set'", setting);
    return 1;
}

int panel_disable(const char *setting) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        panel_list_settings();
        return 2;
    }

    if (strcmp(setting, PANEL_SETTING_AUTOHIDE) == 0) {
        if (tint2_set_autohide(0) == 0) {
            print_success("Autohide disabled");
            return 0;
        }
        print_error("Failed to disable autohide");
        return 1;
    }

    if (strcmp(setting, PANEL_SETTING_SYSTRAY) == 0) {
        if (tint2_set_systray(0) == 0) {
            print_success("Systray disabled");
            return 0;
        }
        return 1;
    }

    print_error("Setting '%s' does not support enable/disable, use 'set'", setting);
    return 1;
}

int panel_set(const char *setting, const char *value) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        panel_list_settings();
        return 2;
    }

    if (strcmp(setting, PANEL_SETTING_POSITION) == 0) {
        if (tint2_set_position(value) == 0) {
            print_success("Panel position set to '%s'", value);
            return 0;
        }
        return 1;
    }

    print_error("Setting '%s' does not support 'set', use enable/disable", setting);
    return 1;
}

int panel_status(const char *setting) {
    if (setting) {
        if (!is_valid_setting(setting)) {
            print_error("Unknown setting '%s'", setting);
            panel_list_settings();
            return 2;
        }

        if (strcmp(setting, PANEL_SETTING_POSITION) == 0) {
            char *pos = tint2_get_position();
            printf("%s\n", pos ? pos : "unknown");
            free(pos);
            return 0;
        }
        if (strcmp(setting, PANEL_SETTING_AUTOHIDE) == 0) {
            int val = tint2_get_autohide();
            printf("%s\n", val == 1 ? "enabled" : val == 0 ? "disabled" : "unknown");
            return 0;
        }
        if (strcmp(setting, PANEL_SETTING_SYSTRAY) == 0) {
            int val = tint2_get_systray();
            printf("%s\n", val == 1 ? "enabled" : val == 0 ? "disabled" : "unknown");
            return 0;
        }
    }

    // Show all panel status
    print_header("Panel (tint2)");

    char *position = tint2_get_position();
    int autohide = tint2_get_autohide();
    int systray = tint2_get_systray();

    printf("  Position: %s\n", position ? position : "unknown");
    printf("  Autohide: %s\n", autohide == 1 ? "enabled" : autohide == 0 ? "disabled" : "unknown");
    printf("  Systray:  %s\n", systray == 1 ? "enabled" : systray == 0 ? "disabled" : "unknown");

    free(position);
    return 0;
}
