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
    (void)argv;  /* unused for now */

    if (argc < 2) {
        print_usage();
        return 0;
    }

    printf("opende: not yet implemented\n");
    return 1;
}
