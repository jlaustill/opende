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
