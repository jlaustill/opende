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
