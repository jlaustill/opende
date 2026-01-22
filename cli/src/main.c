// cli/src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/output.h"
#include "categories/input.h"
#include "categories/effects.h"
#include "categories/panel.h"
#include "ui/menu.h"

#define VERSION "0.1.0"

// Exit codes
#define EXIT_SUCCESS_CODE 0
#define EXIT_ERROR        1
#define EXIT_NOT_FOUND    2
#define EXIT_PERMISSION   3

typedef enum {
    CAT_NONE,
    CAT_INPUT,
    CAT_EFFECTS,
    CAT_PANEL
} Category;

typedef enum {
    ACT_NONE,
    ACT_ENABLE,
    ACT_DISABLE,
    ACT_SET,
    ACT_STATUS
} Action;

static void print_usage(void) {
    printf("Usage: opende <category> <action> [setting] [value]\n");
    printf("       opende config           Interactive mode\n");
    printf("       opende status           Show all settings\n");
    printf("       opende --version        Show version\n");
    printf("\nCategories:\n");
    printf("  input    Input device settings (scrolling, tap-to-click)\n");
    printf("  effects  Compositor settings (shadows, transparency)\n");
    printf("  panel    Panel/UI settings (position, autohide)\n");
    printf("\nActions:\n");
    printf("  enable <setting>       Turn a setting on\n");
    printf("  disable <setting>      Turn a setting off\n");
    printf("  set <setting> <value>  Set a specific value\n");
    printf("  status [setting]       Show current state\n");
    printf("\nExamples:\n");
    printf("  opende input enable natural-scrolling\n");
    printf("  opende effects disable shadows\n");
    printf("  opende panel set position bottom\n");
    printf("  opende status\n");
}

static Category parse_category(const char *str) {
    if (strcmp(str, "input") == 0) return CAT_INPUT;
    if (strcmp(str, "effects") == 0) return CAT_EFFECTS;
    if (strcmp(str, "panel") == 0) return CAT_PANEL;
    return CAT_NONE;
}

static Action parse_action(const char *str) {
    if (strcmp(str, "enable") == 0) return ACT_ENABLE;
    if (strcmp(str, "disable") == 0) return ACT_DISABLE;
    if (strcmp(str, "set") == 0) return ACT_SET;
    if (strcmp(str, "status") == 0) return ACT_STATUS;
    return ACT_NONE;
}

static int handle_status_all(void) {
    input_status(NULL);
    effects_status(NULL);
    panel_status(NULL);
    return EXIT_SUCCESS_CODE;
}

static int handle_config_interactive(void) {
    return menu_run();
}

static int handle_category(Category cat, Action act, const char *setting, const char *value) {
    if (cat == CAT_INPUT) {
        switch (act) {
            case ACT_ENABLE:  return input_enable(setting);
            case ACT_DISABLE: return input_disable(setting);
            case ACT_SET:     return input_set(setting, value);
            case ACT_STATUS:  return input_status(setting);
            default:          return EXIT_ERROR;
        }
    }

    if (cat == CAT_EFFECTS) {
        switch (act) {
            case ACT_ENABLE:  return effects_enable(setting);
            case ACT_DISABLE: return effects_disable(setting);
            case ACT_SET:     return effects_set(setting, value);
            case ACT_STATUS:  return effects_status(setting);
            default:          return EXIT_ERROR;
        }
    }

    if (cat == CAT_PANEL) {
        switch (act) {
            case ACT_ENABLE:  return panel_enable(setting);
            case ACT_DISABLE: return panel_disable(setting);
            case ACT_SET:     return panel_set(setting, value);
            case ACT_STATUS:  return panel_status(setting);
            default:          return EXIT_ERROR;
        }
    }

    print_error("Category not yet implemented");
    return EXIT_ERROR;
}

int main(int argc, char *argv[]) {
    output_init();

    if (argc < 2) {
        print_usage();
        return EXIT_SUCCESS_CODE;
    }

    // Handle special commands
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        printf("opende %s\n", VERSION);
        return EXIT_SUCCESS_CODE;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage();
        return EXIT_SUCCESS_CODE;
    }

    if (strcmp(argv[1], "status") == 0) {
        return handle_status_all();
    }

    if (strcmp(argv[1], "config") == 0) {
        return handle_config_interactive();
    }

    // Parse category
    Category cat = parse_category(argv[1]);
    if (cat == CAT_NONE) {
        print_error("Unknown category '%s'", argv[1]);
        printf("Available categories: input, effects, panel\n");
        return EXIT_NOT_FOUND;
    }

    // If only category given, show status for that category
    if (argc == 2) {
        return handle_category(cat, ACT_STATUS, NULL, NULL);
    }

    // Parse action
    Action act = parse_action(argv[2]);
    if (act == ACT_NONE) {
        print_error("Unknown action '%s'", argv[2]);
        printf("Available actions: enable, disable, set, status\n");
        return EXIT_NOT_FOUND;
    }

    // Get setting and value
    const char *setting = argc > 3 ? argv[3] : NULL;
    const char *value = argc > 4 ? argv[4] : NULL;

    // Validate arguments
    if ((act == ACT_ENABLE || act == ACT_DISABLE) && !setting) {
        print_error("Setting name required for %s", argv[2]);
        return EXIT_ERROR;
    }

    if (act == ACT_SET && (!setting || !value)) {
        print_error("Both setting and value required for 'set'");
        return EXIT_ERROR;
    }

    return handle_category(cat, act, setting, value);
}
