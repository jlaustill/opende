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
