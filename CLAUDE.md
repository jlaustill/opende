# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

OpenDE is a minimal, lightweight Linux desktop environment. This is a **configuration distribution**, not a compiled software project - it consists of shell scripts and configuration files that compose existing Linux desktop components into a cohesive experience.

## Architecture

OpenDE assembles these components:

| Component | Purpose | Config Location |
|-----------|---------|-----------------|
| openbox | Window Manager | `config/rc.xml` |
| tint2 | Panel/Taskbar | user's ~/.config/tint2/ |
| jgmenu | Application Menu | user's ~/.config/jgmenu/ |
| picom | Compositor | `config/picom.conf` |
| dunst | Notifications | `config/dunstrc` |
| nitrogen | Wallpaper | user's ~/.config/nitrogen/ |
| pcmanfm | File Manager | daemon mode for automounting |

**Session startup flow** (`scripts/opende-session`):
1. Set environment variables (XDG_CURRENT_DESKTOP, etc.)
2. Start picom compositor
3. Restore wallpaper (nitrogen)
4. Start dunst notifications
5. Start polkit agent + systray apps
6. Start tint2 panel
7. Run autostart entries (dex)
8. Exec openbox (blocks until logout)

## Commands

**Install dependencies:**
```bash
sudo apt install openbox picom dunst nitrogen pcmanfm lxpolkit pasystray network-manager-gnome dex tint2 jgmenu
```

**Reload configurations without logout:**
```bash
openbox --reconfigure      # Reload window manager (rc.xml changes)
killall -SIGUSR1 tint2     # Reload panel
```

**Test notifications:**
```bash
notify-send "Title" "Message"
```

**To use:** Select "OpenDE" from login manager session menu.

## Key Files

- `scripts/opende-session` - Main session startup script (must be executable)
- `sessions/opende.desktop` - Login manager entry file
- `config/rc.xml` - Openbox keybindings, mouse actions, window rules, theming
- `config/picom.conf` - Shadows, fade animations, transparency
- `config/dunstrc` - Notification appearance and behavior

## Notes

- No build system, tests, or linting - pure configuration files
- `sessions/opende.desktop` contains an absolute path that must match the actual install location
- Users copy configs to `~/.config/` for personal modifications
