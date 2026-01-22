// cli/src/categories/input.c
#include "input.h"
#include "../backends/xorg_conf.h"
#include "../util/output.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *valid_settings[] = {
    INPUT_SETTING_NATURAL_SCROLL,
    INPUT_SETTING_TAP_CLICK,
    INPUT_SETTING_MOUSE_ACCEL,
    NULL
};

static int is_valid_setting(const char *setting) {
    for (int i = 0; valid_settings[i]; i++) {
        if (strcmp(setting, valid_settings[i]) == 0) return 1;
    }
    return 0;
}

void input_list_settings(void) {
    printf("Available input settings:\n");
    for (int i = 0; valid_settings[i]; i++) {
        printf("  %s\n", valid_settings[i]);
    }
}

int input_enable(const char *setting) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        input_list_settings();
        return 2;
    }

    if (strcmp(setting, INPUT_SETTING_NATURAL_SCROLL) == 0) {
        if (xorg_set_natural_scroll(1) == 0) {
            print_success("Natural scrolling enabled");
            return 0;
        }
        return 1;
    }

    if (strcmp(setting, INPUT_SETTING_TAP_CLICK) == 0) {
        if (xorg_set_tap_click(1) == 0) {
            print_success("Tap-to-click enabled");
            return 0;
        }
        return 1;
    }

    print_error("Setting '%s' does not support enable/disable, use 'set'", setting);
    return 1;
}

int input_disable(const char *setting) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        input_list_settings();
        return 2;
    }

    if (strcmp(setting, INPUT_SETTING_NATURAL_SCROLL) == 0) {
        if (xorg_set_natural_scroll(0) == 0) {
            print_success("Natural scrolling disabled");
            return 0;
        }
        return 1;
    }

    if (strcmp(setting, INPUT_SETTING_TAP_CLICK) == 0) {
        if (xorg_set_tap_click(0) == 0) {
            print_success("Tap-to-click disabled");
            return 0;
        }
        return 1;
    }

    print_error("Setting '%s' does not support enable/disable, use 'set'", setting);
    return 1;
}

int input_set(const char *setting, const char *value) {
    if (!is_valid_setting(setting)) {
        print_error("Unknown setting '%s'", setting);
        input_list_settings();
        return 2;
    }

    if (strcmp(setting, INPUT_SETTING_MOUSE_ACCEL) == 0) {
        if (xorg_set_mouse_accel(value) == 0) {
            print_success("Mouse acceleration set to '%s'", value);
            return 0;
        }
        return 1;
    }

    print_error("Setting '%s' does not support 'set', use enable/disable", setting);
    return 1;
}

int input_status(const char *setting) {
    if (setting) {
        if (!is_valid_setting(setting)) {
            print_error("Unknown setting '%s'", setting);
            input_list_settings();
            return 2;
        }

        if (strcmp(setting, INPUT_SETTING_NATURAL_SCROLL) == 0) {
            int val = xorg_get_natural_scroll();
            printf("%s\n", val == 1 ? "enabled" : val == 0 ? "disabled" : "default");
            return 0;
        }
        if (strcmp(setting, INPUT_SETTING_TAP_CLICK) == 0) {
            int val = xorg_get_tap_click();
            printf("%s\n", val == 1 ? "enabled" : val == 0 ? "disabled" : "default");
            return 0;
        }
        if (strcmp(setting, INPUT_SETTING_MOUSE_ACCEL) == 0) {
            char *val = xorg_get_mouse_accel();
            printf("%s\n", val);
            free(val);
            return 0;
        }
    }

    // Show all input status
    print_header("Input (system-level, requires sudo)");

    int natural = xorg_get_natural_scroll();
    int tap = xorg_get_tap_click();
    char *accel = xorg_get_mouse_accel();

    printf("  Natural scrolling: %s\n",
           natural == 1 ? "enabled" : natural == 0 ? "disabled" : "default");
    printf("  Tap-to-click:      %s\n",
           tap == 1 ? "enabled" : tap == 0 ? "disabled" : "default");
    printf("  Mouse acceleration: %s\n", accel);

    free(accel);
    return 0;
}
