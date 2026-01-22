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
