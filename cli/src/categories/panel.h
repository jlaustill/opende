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
