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
