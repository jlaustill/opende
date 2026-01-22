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
