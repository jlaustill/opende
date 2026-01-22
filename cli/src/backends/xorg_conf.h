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
