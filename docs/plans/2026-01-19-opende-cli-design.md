# OpenDE CLI Design

**Date:** 2026-01-19
**Status:** Approved

## Overview

A unified command-line tool for managing OpenDE user preferences. The CLI provides both scriptable subcommands and an interactive menu for discovery.

## Goals

- Simple interface for toggling common preferences
- Abstract away config file locations and formats
- Swappable backends (change compositor/panel without changing user interface)
- Zero dependencies for basic operation

## Non-Goals

- Installation/setup (separate workflow)
- Window manager configuration (too WM-specific)
- System administration tasks

## Command Structure

```
opende <category> <action> [setting]
opende config                         # Interactive mode
opende status                         # Show all settings
```

### Categories

| Category | Description |
|----------|-------------|
| `input` | Input device settings (scrolling, tap-to-click, acceleration) |
| `effects` | Compositor/visual settings (shadows, transparency, animations) |
| `panel` | Panel/UI settings (position, autohide, systray) |

### Actions

| Action | Description |
|--------|-------------|
| `enable <setting>` | Turn a setting on |
| `disable <setting>` | Turn a setting off |
| `set <setting> <value>` | Set a setting to a specific value |
| `status [setting]` | Show current state (all or specific) |

### Examples

```bash
# Scripting style
opende input enable natural-scrolling
opende input disable tap-to-click
opende effects enable shadows
opende panel set position bottom

# Check status
opende status
opende input status

# Interactive mode
opende config              # Full menu
opende input               # Input submenu
```

## Settings Map

### Input Settings (`opende input`)

| Setting | Config Location | Level |
|---------|-----------------|-------|
| `natural-scrolling` | `/etc/X11/xorg.conf.d/40-opende-input.conf` | System |
| `tap-to-click` | `/etc/X11/xorg.conf.d/40-opende-input.conf` | System |
| `mouse-accel` | `/etc/X11/xorg.conf.d/40-opende-input.conf` | System |

### Visual Effects (`opende effects`)

| Setting | Config Location | Level |
|---------|-----------------|-------|
| `compositor` | Starts/stops picom process | Session |
| `shadows` | `~/.config/opende/picom.conf` | User |
| `transparency` | `~/.config/opende/picom.conf` | User |
| `animations` | `~/.config/opende/picom.conf` | User |

### Panel/UI (`opende panel`)

| Setting | Config Location | Level |
|---------|-----------------|-------|
| `position` | `~/.config/opende/tint2rc` | User |
| `autohide` | `~/.config/opende/tint2rc` | User |
| `systray` | `~/.config/opende/tint2rc` | User |

## Internal Architecture

```
opende/
├── main.c              # Argument parsing, dispatch to category
├── categories/
│   ├── input.c         # Input settings logic
│   ├── effects.c       # Compositor/visual settings
│   └── panel.c         # Panel/UI settings
├── backends/
│   ├── xorg_conf.c     # Read/write xorg.conf.d files
│   ├── picom.c         # Modify picom.conf, reload picom
│   └── tint2.c         # Modify tint2rc, send reload signal
├── ui/
│   ├── menu.c          # Simple text menu system
│   └── output.c        # Consistent status/error formatting
└── util/
    ├── config.c        # Path resolution, XDG handling
    └── privilege.c     # Sudo detection, privilege escalation
```

### Design Principles

- **Categories** know *what* to change
- **Backends** know *how* to change it
- Adding a new backend (e.g., polybar) doesn't touch category code
- All user-facing output goes through `output.c` for consistency
- `privilege.c` handles sudo escalation cleanly

## Interactive Menu

Simple numbered menus that work everywhere:

```
$ opende config

OpenDE Preferences

1) Input settings
2) Visual effects
3) Panel/UI
0) Exit

Choose [0-3]: 1

Input Settings

  [ON]  Natural scrolling
  [OFF] Tap-to-click
  [MED] Mouse acceleration

1) Toggle natural scrolling *
2) Toggle tap-to-click *
3) Set mouse acceleration *
0) Back

Choose [0-3]:
```

- Settings requiring sudo marked with `*`
- Current state shown with `[ON]`/`[OFF]` indicators
- Menu redisplays after changes with updated state
- `Ctrl+C` exits cleanly

### Future: Rich TUI

Menu logic is separated from rendering. Future enhancement can add:
- `ui/menu_gum.c` - Rich TUI using `gum`
- `ui/menu_dialog.c` - TUI using `dialog`/`whiptail`
- Runtime detection: use fancy renderer if available and stdout is TTY

## Output & Exit Codes

### Human-Friendly Output

```
$ opende status

OpenDE Configuration Status

Input (system-level):
  Natural scrolling:  enabled
  Tap-to-click:       disabled
  Mouse acceleration: medium

Effects (user-level):
  Compositor:   running
  Shadows:      enabled
  Transparency: 90%
  Animations:   enabled

Panel (user-level):
  Position: bottom
  Autohide: disabled
  Systray:  nm-applet, pasystray, blueman-applet
```

### Script-Friendly Output

```bash
$ opende input status natural-scrolling
enabled
```

### Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | General error |
| 2 | Setting not found |
| 3 | Permission denied |

## Error Handling

### Permission Errors

```
$ opende input enable natural-scrolling
Error: This setting requires root privileges.
Run: sudo opende input enable natural-scrolling
```

Or with escalation prompt:
```
$ opende input enable natural-scrolling
This setting requires root privileges. Elevate with sudo? [y/N]: y
```

### Missing Dependencies

```
$ opende effects enable shadows
Error: picom is not installed.
Install with: sudo apt install picom
```

### Config Conflicts

```
$ opende input enable natural-scrolling
Warning: Existing config found at /etc/X11/xorg.conf.d/99-custom.conf
This may conflict with OpenDE settings. Proceed anyway? [y/N]:
```

### Unknown Settings

```
$ opende input enable turbo-mode
Error: Unknown setting 'turbo-mode'
Available input settings: natural-scrolling, tap-to-click, mouse-accel
```

## Implementation Notes

- **Language:** C
- **Build system:** TBD (deferred)
- **Distribution:** TBD (deferred)

## Open Questions

- Should the CLI auto-detect available backends (e.g., polybar vs tint2)?
- Should there be a `opende reset` command to restore defaults?
- Config file format for storing user choices vs actual app configs?
