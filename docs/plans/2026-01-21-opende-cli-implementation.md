# OpenDE CLI Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build a C command-line tool for managing OpenDE user preferences with both scriptable commands and interactive menus.

**Architecture:** Single-binary CLI using modular C with categories (input/effects/panel) that delegate to backends (xorg/picom/tint2). All output through consistent formatting functions. Simple Makefile build.

**Tech Stack:** C99, Make, standard POSIX APIs

---

## Phase 1: Project Skeleton & Build System

### Task 1: Create directory structure and Makefile

**Files:**
- Create: `cli/Makefile`
- Create: `cli/src/main.c`

**Step 1: Create the cli directory structure**

```bash
mkdir -p cli/src cli/src/categories cli/src/backends cli/src/ui cli/src/util
```

**Step 2: Create minimal main.c**

```c
// cli/src/main.c
#include <stdio.h>
#include <stdlib.h>

#define VERSION "0.1.0"

static void print_usage(void) {
    printf("Usage: opende <category> <action> [setting] [value]\n");
    printf("       opende config           Interactive mode\n");
    printf("       opende status           Show all settings\n");
    printf("\nCategories: input, effects, panel\n");
    printf("Actions: enable, disable, set, status\n");
    printf("\nVersion: %s\n", VERSION);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 0;
    }

    printf("opende: not yet implemented\n");
    return 1;
}
```

**Step 3: Create Makefile**

```makefile
# cli/Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
BIN = opende

SRCS = $(wildcard $(SRC_DIR)/*.c) \
       $(wildcard $(SRC_DIR)/categories/*.c) \
       $(wildcard $(SRC_DIR)/backends/*.c) \
       $(wildcard $(SRC_DIR)/ui/*.c) \
       $(wildcard $(SRC_DIR)/util/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean install

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN)

install: $(BIN)
	install -m 755 $(BIN) /usr/local/bin/
```

**Step 4: Build and test**

```bash
cd cli && make
./opende
./opende --help
```

Expected: Binary compiles, shows usage when run without args.

**Step 5: Commit**

```bash
git add cli/
git commit -m "feat(cli): add project skeleton and build system"
```

---

### Task 2: Add output formatting utilities

**Files:**
- Create: `cli/src/util/output.h`
- Create: `cli/src/util/output.c`

**Step 1: Create output.h**

```c
// cli/src/util/output.h
#ifndef OPENDE_OUTPUT_H
#define OPENDE_OUTPUT_H

// ANSI color codes (disabled if not a TTY)
void output_init(void);

// Formatted output functions
void print_info(const char *fmt, ...);
void print_error(const char *fmt, ...);
void print_warn(const char *fmt, ...);
void print_success(const char *fmt, ...);

// Status display helpers
void print_setting(const char *name, const char *value, int enabled);
void print_header(const char *title);

#endif
```

**Step 2: Create output.c**

```c
// cli/src/util/output.c
#include "output.h"
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

static int use_colors = 0;

#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_RESET   "\033[0m"

void output_init(void) {
    use_colors = isatty(STDOUT_FILENO);
}

void print_info(const char *fmt, ...) {
    va_list args;
    if (use_colors) printf("%s[INFO]%s ", COLOR_BLUE, COLOR_RESET);
    else printf("[INFO] ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void print_error(const char *fmt, ...) {
    va_list args;
    if (use_colors) fprintf(stderr, "%s[ERROR]%s ", COLOR_RED, COLOR_RESET);
    else fprintf(stderr, "[ERROR] ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void print_warn(const char *fmt, ...) {
    va_list args;
    if (use_colors) printf("%s[WARN]%s ", COLOR_YELLOW, COLOR_RESET);
    else printf("[WARN] ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void print_success(const char *fmt, ...) {
    va_list args;
    if (use_colors) printf("%s[OK]%s ", COLOR_GREEN, COLOR_RESET);
    else printf("[OK] ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void print_setting(const char *name, const char *value, int enabled) {
    const char *indicator;
    if (use_colors) {
        indicator = enabled ? COLOR_GREEN "[ON] " COLOR_RESET
                           : COLOR_RED "[OFF]" COLOR_RESET;
    } else {
        indicator = enabled ? "[ON] " : "[OFF]";
    }
    printf("  %s %s: %s\n", indicator, name, value);
}

void print_header(const char *title) {
    printf("\n%s\n", title);
    for (int i = 0; title[i]; i++) printf("-");
    printf("\n\n");
}
```

**Step 3: Build and verify**

```bash
cd cli && make clean && make
```

Expected: Compiles without warnings.

**Step 4: Commit**

```bash
git add cli/src/util/
git commit -m "feat(cli): add output formatting utilities"
```

---

### Task 3: Add config path utilities

**Files:**
- Create: `cli/src/util/config.h`
- Create: `cli/src/util/config.c`

**Step 1: Create config.h**

```c
// cli/src/util/config.h
#ifndef OPENDE_CONFIG_H
#define OPENDE_CONFIG_H

// Get path to user config file, creating directory if needed
// Returns allocated string (caller must free) or NULL on error
char *config_get_user_path(const char *filename);

// Get path to system config file
// Returns allocated string (caller must free) or NULL on error
char *config_get_system_path(const char *filename);

// Ensure directory exists for a file path
// Returns 0 on success, -1 on error
int config_ensure_dir(const char *filepath);

// Check if file exists and is readable
int config_file_exists(const char *path);

#endif
```

**Step 2: Create config.c**

```c
// cli/src/util/config.c
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
```

**Step 3: Build and verify**

```bash
cd cli && make clean && make
```

Expected: Compiles without warnings.

**Step 4: Commit**

```bash
git add cli/src/util/config.*
git commit -m "feat(cli): add config path utilities"
```

---

### Task 4: Add argument parsing and dispatch

**Files:**
- Modify: `cli/src/main.c`

**Step 1: Rewrite main.c with proper argument parsing**

```c
// cli/src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/output.h"

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
    print_header("OpenDE Configuration Status");
    printf("(Not yet implemented)\n");
    return EXIT_SUCCESS_CODE;
}

static int handle_config_interactive(void) {
    print_header("OpenDE Preferences");
    printf("(Interactive mode not yet implemented)\n");
    return EXIT_SUCCESS_CODE;
}

static int handle_category(Category cat, Action act, const char *setting, const char *value) {
    const char *cat_name = cat == CAT_INPUT ? "input"
                         : cat == CAT_EFFECTS ? "effects"
                         : "panel";

    printf("Category: %s, Action: %d, Setting: %s, Value: %s\n",
           cat_name, act, setting ? setting : "(none)", value ? value : "(none)");
    printf("(Not yet implemented)\n");
    return EXIT_SUCCESS_CODE;
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
```

**Step 2: Build and test**

```bash
cd cli && make clean && make
./opende
./opende --version
./opende status
./opende input
./opende input enable natural-scrolling
./opende effects set shadows enabled
./opende invalid
```

Expected: All commands parse correctly and show placeholder output.

**Step 3: Commit**

```bash
git add cli/src/main.c
git commit -m "feat(cli): add argument parsing and command dispatch"
```

---

## Phase 2: Effects Category (picom backend)

### Task 5: Create effects category skeleton

**Files:**
- Create: `cli/src/categories/effects.h`
- Create: `cli/src/categories/effects.c`

**Step 1: Create effects.h**

```c
// cli/src/categories/effects.h
#ifndef OPENDE_EFFECTS_H
#define OPENDE_EFFECTS_H

// Available settings
#define EFFECTS_SETTING_COMPOSITOR   "compositor"
#define EFFECTS_SETTING_SHADOWS      "shadows"
#define EFFECTS_SETTING_TRANSPARENCY "transparency"
#define EFFECTS_SETTING_ANIMATIONS   "animations"

// Actions
int effects_enable(const char *setting);
int effects_disable(const char *setting);
int effects_set(const char *setting, const char *value);
int effects_status(const char *setting);  // NULL = all settings

// List available settings
void effects_list_settings(void);

#endif
```

**Step 2: Create effects.c**

```c
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
```

**Step 3: Wire up to main.c**

Add include at top of main.c:
```c
#include "categories/effects.h"
```

Update handle_category function:
```c
static int handle_category(Category cat, Action act, const char *setting, const char *value) {
    if (cat == CAT_EFFECTS) {
        switch (act) {
            case ACT_ENABLE:  return effects_enable(setting);
            case ACT_DISABLE: return effects_disable(setting);
            case ACT_SET:     return effects_set(setting, value);
            case ACT_STATUS:  return effects_status(setting);
            default:          return EXIT_ERROR;
        }
    }

    // Other categories not yet implemented
    print_error("Category not yet implemented");
    return EXIT_ERROR;
}
```

**Step 4: Build and test**

```bash
cd cli && make clean && make
./opende effects status
./opende effects enable shadows
./opende effects enable invalid-setting
```

Expected: Effects commands work, invalid settings show error with available options.

**Step 5: Commit**

```bash
git add cli/src/
git commit -m "feat(cli): add effects category skeleton"
```

---

### Task 6: Implement picom backend - compositor control

**Files:**
- Create: `cli/src/backends/picom.h`
- Create: `cli/src/backends/picom.c`
- Modify: `cli/src/categories/effects.c`

**Step 1: Create picom.h**

```c
// cli/src/backends/picom.h
#ifndef OPENDE_PICOM_H
#define OPENDE_PICOM_H

// Check if picom is installed
int picom_is_installed(void);

// Check if picom is currently running
int picom_is_running(void);

// Start picom with OpenDE config
int picom_start(void);

// Stop picom
int picom_stop(void);

// Reload picom config (restart)
int picom_reload(void);

// Config file operations
int picom_get_shadows(void);           // Returns 1=on, 0=off, -1=error
int picom_set_shadows(int enabled);

int picom_get_animations(void);
int picom_set_animations(int enabled);

int picom_get_transparency(void);      // Returns percentage (0-100) or -1
int picom_set_transparency(int percent);

#endif
```

**Step 2: Create picom.c**

```c
// cli/src/backends/picom.c
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
    system(cmd);

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
```

**Step 3: Update effects.c to use picom backend**

```c
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
```

**Step 4: Build and test**

```bash
cd cli && make clean && make
./opende effects status
./opende effects enable shadows
./opende effects status shadows
```

Expected: Commands work, showing real compositor status and modifying config.

**Step 5: Commit**

```bash
git add cli/src/
git commit -m "feat(cli): implement picom backend for effects category"
```

---

## Phase 3: Panel Category (tint2 backend)

### Task 7: Create panel category and tint2 backend

**Files:**
- Create: `cli/src/categories/panel.h`
- Create: `cli/src/categories/panel.c`
- Create: `cli/src/backends/tint2.h`
- Create: `cli/src/backends/tint2.c`
- Modify: `cli/src/main.c`

**Step 1: Create panel.h**

```c
// cli/src/categories/panel.h
#ifndef OPENDE_PANEL_H
#define OPENDE_PANEL_H

#define PANEL_SETTING_POSITION  "position"
#define PANEL_SETTING_AUTOHIDE  "autohide"
#define PANEL_SETTING_SYSTRAY   "systray"

int panel_enable(const char *setting);
int panel_disable(const char *setting);
int panel_set(const char *setting, const char *value);
int panel_status(const char *setting);

void panel_list_settings(void);

#endif
```

**Step 2: Create tint2.h**

```c
// cli/src/backends/tint2.h
#ifndef OPENDE_TINT2_H
#define OPENDE_TINT2_H

int tint2_is_installed(void);
int tint2_is_running(void);
int tint2_reload(void);

// Position: "top" or "bottom"
char *tint2_get_position(void);  // Returns allocated string, caller frees
int tint2_set_position(const char *position);

// Autohide
int tint2_get_autohide(void);    // 1=on, 0=off, -1=error
int tint2_set_autohide(int enabled);

// Systray
int tint2_get_systray(void);     // 1=on, 0=off, -1=error
int tint2_set_systray(int enabled);

#endif
```

**Step 3: Create tint2.c**

```c
// cli/src/backends/tint2.c
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
        system(cmd);
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
    char *config = get_config_path();
    if (!config) return -1;

    // This is tricky - need to add/remove 'S' from panel_items
    // For now, just warn that manual edit is needed
    print_warn("Systray toggle requires manual edit of %s", config);
    printf("Look for 'panel_items' line and add/remove 'S'\n");

    free(config);
    return -1;
}
```

**Step 4: Create panel.c**

```c
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
    print_header("Panel (user-level)");

    char *position = tint2_get_position();
    int autohide = tint2_get_autohide();
    int systray = tint2_get_systray();

    printf("  Position: %s\n", position ? position : "unknown");
    printf("  Autohide: %s\n", autohide == 1 ? "enabled" : autohide == 0 ? "disabled" : "unknown");
    printf("  Systray:  %s\n", systray == 1 ? "enabled" : systray == 0 ? "disabled" : "unknown");

    free(position);
    return 0;
}
```

**Step 5: Update main.c to include panel category**

Add include:
```c
#include "categories/panel.h"
```

Update handle_category:
```c
static int handle_category(Category cat, Action act, const char *setting, const char *value) {
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
```

**Step 6: Build and test**

```bash
cd cli && make clean && make
./opende panel status
./opende panel set position top
./opende panel enable autohide
```

**Step 7: Commit**

```bash
git add cli/src/
git commit -m "feat(cli): implement panel category with tint2 backend"
```

---

## Phase 4: Input Category (xorg backend)

### Task 8: Create input category and xorg backend

**Files:**
- Create: `cli/src/categories/input.h`
- Create: `cli/src/categories/input.c`
- Create: `cli/src/backends/xorg_conf.h`
- Create: `cli/src/backends/xorg_conf.c`
- Modify: `cli/src/main.c`

**Step 1: Create input.h**

```c
// cli/src/categories/input.h
#ifndef OPENDE_INPUT_H
#define OPENDE_INPUT_H

#define INPUT_SETTING_NATURAL_SCROLL "natural-scrolling"
#define INPUT_SETTING_TAP_CLICK      "tap-to-click"
#define INPUT_SETTING_MOUSE_ACCEL    "mouse-accel"

int input_enable(const char *setting);
int input_disable(const char *setting);
int input_set(const char *setting, const char *value);
int input_status(const char *setting);

void input_list_settings(void);

#endif
```

**Step 2: Create xorg_conf.h**

```c
// cli/src/backends/xorg_conf.h
#ifndef OPENDE_XORG_CONF_H
#define OPENDE_XORG_CONF_H

// Check if we can write to xorg.conf.d (need root)
int xorg_can_write(void);

// Natural scrolling (touchpad)
int xorg_get_natural_scroll(void);
int xorg_set_natural_scroll(int enabled);

// Tap to click (touchpad)
int xorg_get_tap_click(void);
int xorg_set_tap_click(int enabled);

// Mouse acceleration: "off", "low", "medium", "high"
char *xorg_get_mouse_accel(void);
int xorg_set_mouse_accel(const char *level);

#endif
```

**Step 3: Create xorg_conf.c**

```c
// cli/src/backends/xorg_conf.c
#include "xorg_conf.h"
#include "../util/output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define XORG_CONF_DIR "/etc/X11/xorg.conf.d"
#define OPENDE_CONF "40-opende-input.conf"

int xorg_can_write(void) {
    return access(XORG_CONF_DIR, W_OK) == 0;
}

static char *get_config_path(void) {
    static char path[256];
    snprintf(path, sizeof(path), "%s/%s", XORG_CONF_DIR, OPENDE_CONF);
    return path;
}

static int write_config(const char *content) {
    if (!xorg_can_write()) {
        print_error("Permission denied. Run with sudo.");
        return -1;
    }

    char *path = get_config_path();
    FILE *fp = fopen(path, "w");
    if (!fp) {
        print_error("Cannot write to %s", path);
        return -1;
    }

    fprintf(fp, "%s", content);
    fclose(fp);

    print_warn("Changes require X restart or re-login to take effect");
    return 0;
}

// Read current config or return defaults
static int read_option(const char *option) {
    char *path = get_config_path();
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;  // No config = default

    char line[256];
    int result = -1;

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, option)) {
            if (strstr(line, "\"true\"") || strstr(line, "\"on\"") || strstr(line, "\"1\"")) {
                result = 1;
            } else if (strstr(line, "\"false\"") || strstr(line, "\"off\"") || strstr(line, "\"0\"")) {
                result = 0;
            }
            break;
        }
    }

    fclose(fp);
    return result;
}

int xorg_get_natural_scroll(void) {
    return read_option("NaturalScrolling");
}

int xorg_set_natural_scroll(int enabled) {
    char content[1024];
    snprintf(content, sizeof(content),
        "# OpenDE Input Configuration\n"
        "# Generated by opende CLI - do not edit manually\n\n"
        "Section \"InputClass\"\n"
        "    Identifier \"OpenDE touchpad\"\n"
        "    MatchIsTouchpad \"on\"\n"
        "    Driver \"libinput\"\n"
        "    Option \"NaturalScrolling\" \"%s\"\n"
        "EndSection\n",
        enabled ? "true" : "false");

    return write_config(content);
}

int xorg_get_tap_click(void) {
    return read_option("Tapping");
}

int xorg_set_tap_click(int enabled) {
    char content[1024];
    snprintf(content, sizeof(content),
        "# OpenDE Input Configuration\n"
        "# Generated by opende CLI - do not edit manually\n\n"
        "Section \"InputClass\"\n"
        "    Identifier \"OpenDE touchpad\"\n"
        "    MatchIsTouchpad \"on\"\n"
        "    Driver \"libinput\"\n"
        "    Option \"Tapping\" \"%s\"\n"
        "EndSection\n",
        enabled ? "on" : "off");

    return write_config(content);
}

char *xorg_get_mouse_accel(void) {
    char *path = get_config_path();
    FILE *fp = fopen(path, "r");
    if (!fp) return strdup("default");

    char line[256];
    char *result = strdup("default");

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "AccelSpeed")) {
            if (strstr(line, "\"-1\"")) {
                free(result);
                result = strdup("off");
            } else if (strstr(line, "\"-0.5\"")) {
                free(result);
                result = strdup("low");
            } else if (strstr(line, "\"0\"")) {
                free(result);
                result = strdup("medium");
            } else if (strstr(line, "\"0.5\"")) {
                free(result);
                result = strdup("high");
            }
            break;
        }
    }

    fclose(fp);
    return result;
}

int xorg_set_mouse_accel(const char *level) {
    const char *accel_value;

    if (strcmp(level, "off") == 0) accel_value = "-1";
    else if (strcmp(level, "low") == 0) accel_value = "-0.5";
    else if (strcmp(level, "medium") == 0) accel_value = "0";
    else if (strcmp(level, "high") == 0) accel_value = "0.5";
    else {
        print_error("Invalid acceleration level. Use: off, low, medium, high");
        return -1;
    }

    char content[1024];
    snprintf(content, sizeof(content),
        "# OpenDE Input Configuration\n"
        "# Generated by opende CLI - do not edit manually\n\n"
        "Section \"InputClass\"\n"
        "    Identifier \"OpenDE pointer\"\n"
        "    MatchIsPointer \"on\"\n"
        "    Driver \"libinput\"\n"
        "    Option \"AccelSpeed\" \"%s\"\n"
        "EndSection\n",
        accel_value);

    return write_config(content);
}
```

**Step 4: Create input.c**

```c
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
```

**Step 5: Update main.c to include input category**

Add include:
```c
#include "categories/input.h"
```

Update handle_category to add input handling (full function):
```c
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
```

Also update handle_status_all to show all categories:
```c
static int handle_status_all(void) {
    input_status(NULL);
    effects_status(NULL);
    panel_status(NULL);
    return EXIT_SUCCESS_CODE;
}
```

**Step 6: Build and test**

```bash
cd cli && make clean && make
./opende status
./opende input status
sudo ./opende input enable natural-scrolling
```

**Step 7: Commit**

```bash
git add cli/src/
git commit -m "feat(cli): implement input category with xorg backend"
```

---

## Phase 5: Interactive Menu

### Task 9: Add interactive menu system

**Files:**
- Create: `cli/src/ui/menu.h`
- Create: `cli/src/ui/menu.c`
- Modify: `cli/src/main.c`

**Step 1: Create menu.h**

```c
// cli/src/ui/menu.h
#ifndef OPENDE_MENU_H
#define OPENDE_MENU_H

// Run the interactive configuration menu
// Returns 0 on clean exit, non-zero on error
int menu_run(void);

#endif
```

**Step 2: Create menu.c**

```c
// cli/src/ui/menu.c
#include "menu.h"
#include "../categories/input.h"
#include "../categories/effects.h"
#include "../categories/panel.h"
#include "../backends/picom.h"
#include "../backends/tint2.h"
#include "../backends/xorg_conf.h"
#include "../util/output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void clear_screen(void) {
    printf("\033[2J\033[H");
}

static int get_choice(int max) {
    printf("\nChoose [0-%d]: ", max);
    fflush(stdout);

    char buf[16];
    if (!fgets(buf, sizeof(buf), stdin)) {
        return -1;  // EOF (Ctrl+D)
    }

    int choice = atoi(buf);
    if (choice < 0 || choice > max) {
        return -1;
    }
    return choice;
}

static void show_input_menu(void) {
    while (1) {
        clear_screen();
        printf("Input Settings\n");
        printf("==============\n\n");

        int natural = xorg_get_natural_scroll();
        int tap = xorg_get_tap_click();
        char *accel = xorg_get_mouse_accel();

        printf("  [%s] Natural scrolling\n",
               natural == 1 ? "ON " : natural == 0 ? "OFF" : "---");
        printf("  [%s] Tap-to-click\n",
               tap == 1 ? "ON " : tap == 0 ? "OFF" : "---");
        printf("  [%s] Mouse acceleration\n\n", accel);

        free(accel);

        printf("1) Toggle natural scrolling *\n");
        printf("2) Toggle tap-to-click *\n");
        printf("3) Set mouse acceleration *\n");
        printf("0) Back\n");
        printf("\n* Requires sudo\n");

        int choice = get_choice(3);

        switch (choice) {
            case 0: return;
            case 1:
                if (natural == 1) input_disable("natural-scrolling");
                else input_enable("natural-scrolling");
                break;
            case 2:
                if (tap == 1) input_disable("tap-to-click");
                else input_enable("tap-to-click");
                break;
            case 3: {
                printf("\nAcceleration (off/low/medium/high): ");
                char buf[32];
                if (fgets(buf, sizeof(buf), stdin)) {
                    buf[strcspn(buf, "\n")] = 0;
                    input_set("mouse-accel", buf);
                }
                break;
            }
            default:
                break;
        }

        printf("\nPress Enter to continue...");
        getchar();
    }
}

static void show_effects_menu(void) {
    while (1) {
        clear_screen();
        printf("Visual Effects\n");
        printf("==============\n\n");

        int compositor = picom_is_running();
        int shadows = picom_get_shadows();
        int animations = picom_get_animations();
        int transparency = picom_get_transparency();

        printf("  [%s] Compositor\n", compositor ? "ON " : "OFF");
        printf("  [%s] Shadows\n", shadows == 1 ? "ON " : shadows == 0 ? "OFF" : "---");
        printf("  [%s] Animations\n", animations == 1 ? "ON " : animations == 0 ? "OFF" : "---");
        printf("  [%3d%%] Transparency\n\n", transparency >= 0 ? transparency : 0);

        printf("1) Toggle compositor\n");
        printf("2) Toggle shadows\n");
        printf("3) Toggle animations\n");
        printf("4) Set transparency\n");
        printf("0) Back\n");

        int choice = get_choice(4);

        switch (choice) {
            case 0: return;
            case 1:
                if (compositor) effects_disable("compositor");
                else effects_enable("compositor");
                break;
            case 2:
                if (shadows == 1) effects_disable("shadows");
                else effects_enable("shadows");
                break;
            case 3:
                if (animations == 1) effects_disable("animations");
                else effects_enable("animations");
                break;
            case 4: {
                printf("\nTransparency (0-100): ");
                char buf[32];
                if (fgets(buf, sizeof(buf), stdin)) {
                    buf[strcspn(buf, "\n")] = 0;
                    effects_set("transparency", buf);
                }
                break;
            }
            default:
                break;
        }

        printf("\nPress Enter to continue...");
        getchar();
    }
}

static void show_panel_menu(void) {
    while (1) {
        clear_screen();
        printf("Panel Settings\n");
        printf("==============\n\n");

        char *position = tint2_get_position();
        int autohide = tint2_get_autohide();
        int systray = tint2_get_systray();

        printf("  [%s] Position\n", position ? position : "---");
        printf("  [%s] Autohide\n", autohide == 1 ? "ON " : autohide == 0 ? "OFF" : "---");
        printf("  [%s] Systray\n\n", systray == 1 ? "ON " : systray == 0 ? "OFF" : "---");

        free(position);

        printf("1) Set position (top/bottom)\n");
        printf("2) Toggle autohide\n");
        printf("3) Toggle systray\n");
        printf("0) Back\n");

        int choice = get_choice(3);

        switch (choice) {
            case 0: return;
            case 1: {
                printf("\nPosition (top/bottom): ");
                char buf[32];
                if (fgets(buf, sizeof(buf), stdin)) {
                    buf[strcspn(buf, "\n")] = 0;
                    panel_set("position", buf);
                }
                break;
            }
            case 2:
                if (autohide == 1) panel_disable("autohide");
                else panel_enable("autohide");
                break;
            case 3:
                if (systray == 1) panel_disable("systray");
                else panel_enable("systray");
                break;
            default:
                break;
        }

        printf("\nPress Enter to continue...");
        getchar();
    }
}

int menu_run(void) {
    while (1) {
        clear_screen();
        printf("OpenDE Preferences\n");
        printf("==================\n\n");

        printf("1) Input settings\n");
        printf("2) Visual effects\n");
        printf("3) Panel/UI\n");
        printf("0) Exit\n");

        int choice = get_choice(3);

        switch (choice) {
            case -1:
            case 0:
                clear_screen();
                return 0;
            case 1:
                show_input_menu();
                break;
            case 2:
                show_effects_menu();
                break;
            case 3:
                show_panel_menu();
                break;
            default:
                break;
        }
    }
}
```

**Step 3: Update main.c to use menu**

Add include:
```c
#include "ui/menu.h"
```

Update handle_config_interactive:
```c
static int handle_config_interactive(void) {
    return menu_run();
}
```

**Step 4: Build and test**

```bash
cd cli && make clean && make
./opende config
```

Expected: Interactive menu appears, navigation works.

**Step 5: Commit**

```bash
git add cli/src/
git commit -m "feat(cli): add interactive configuration menu"
```

---

## Phase 6: Installation Integration

### Task 10: Update project install script

**Files:**
- Modify: `/home/linux/code/opende/install.sh`
- Modify: `/home/linux/code/opende/uninstall.sh`

**Step 1: Update install.sh to build and install CLI**

Add after the config copy section:
```bash
# Build and install CLI tool
info "Building CLI tool..."
if [ -d "$SOURCE_DIR/cli" ]; then
    (cd "$SOURCE_DIR/cli" && make clean && make)
    if [ -f "$SOURCE_DIR/cli/opende" ]; then
        info "Installing CLI tool..."
        install -m 755 "$SOURCE_DIR/cli/opende" /usr/local/bin/
    else
        warn "CLI build failed, skipping CLI installation"
    fi
fi
```

**Step 2: Update uninstall.sh to remove CLI**

Add after removing session files:
```bash
# Remove CLI tool
if [ -f /usr/local/bin/opende ]; then
    info "Removing CLI tool..."
    rm -f /usr/local/bin/opende
fi
```

**Step 3: Test installation**

```bash
sudo ./install.sh
opende --version
opende status
sudo ./uninstall.sh
```

**Step 4: Commit**

```bash
git add install.sh uninstall.sh
git commit -m "feat: integrate CLI tool into install/uninstall scripts"
```

---

## Phase 7: Final Polish

### Task 11: Update documentation

**Files:**
- Modify: `README.md`

**Step 1: Add CLI section to README**

Add after the Keyboard Shortcuts section:
```markdown
## CLI Tool

OpenDE includes a command-line tool for managing settings:

```bash
# Interactive mode
opende config

# Show all settings
opende status

# Manage specific categories
opende effects status
opende effects enable shadows
opende effects set transparency 85

opende panel set position bottom
opende panel enable autohide

opende input enable natural-scrolling    # requires sudo
opende input set mouse-accel medium      # requires sudo
```

### Available Settings

| Category | Setting | Values |
|----------|---------|--------|
| effects | compositor | enable/disable |
| effects | shadows | enable/disable |
| effects | animations | enable/disable |
| effects | transparency | 0-100 |
| panel | position | top/bottom |
| panel | autohide | enable/disable |
| input | natural-scrolling | enable/disable (sudo) |
| input | tap-to-click | enable/disable (sudo) |
| input | mouse-accel | off/low/medium/high (sudo) |
```

**Step 2: Commit**

```bash
git add README.md
git commit -m "docs: add CLI tool documentation to README"
```

---

### Task 12: Final commit and PR

**Step 1: Create feature branch and push**

```bash
git checkout -b feature/cli-tool
git push -u origin feature/cli-tool
```

**Step 2: Create PR**

```bash
gh pr create --title "Add opende CLI tool" --body "$(cat <<'EOF'
## Summary
Implements the opende CLI tool as designed in docs/plans/2026-01-19-opende-cli-design.md

Features:
- Three categories: input, effects, panel
- Scriptable commands: enable, disable, set, status
- Interactive configuration menu
- Colored output with TTY detection
- Proper exit codes

## Components
- `cli/src/main.c` - Argument parsing and dispatch
- `cli/src/categories/` - Input, effects, panel logic
- `cli/src/backends/` - Picom, tint2, xorg config handlers
- `cli/src/ui/menu.c` - Interactive menu system
- `cli/src/util/` - Output formatting, config paths

## Test plan
- [ ] `make` builds without warnings
- [ ] `opende --help` shows usage
- [ ] `opende status` shows all settings
- [ ] `opende effects enable shadows` modifies picom config
- [ ] `opende config` launches interactive menu
- [ ] `sudo ./install.sh` installs CLI to /usr/local/bin
- [ ] Input settings require sudo and warn appropriately

🤖 Generated with [Claude Code](https://claude.com/claude-code)
EOF
)"
```

---

## Summary

**Total Tasks:** 12

**Phase 1 (Skeleton):** Tasks 1-4 - Project structure, build system, utilities, argument parsing
**Phase 2 (Effects):** Tasks 5-6 - Effects category with picom backend
**Phase 3 (Panel):** Task 7 - Panel category with tint2 backend
**Phase 4 (Input):** Task 8 - Input category with xorg backend
**Phase 5 (Menu):** Task 9 - Interactive menu system
**Phase 6 (Install):** Task 10 - Integration with install scripts
**Phase 7 (Polish):** Tasks 11-12 - Documentation and PR

**Estimated Commits:** 12 (one per task)
