# OpenDE

A minimal, lightweight desktop environment for Linux.

## Components

### Window Manager
Controls window placement, decorations (title bars, borders), focus behavior, and keyboard shortcuts. This is the core of any desktop environment.

OpenDE supports multiple window managers. Select your preferred one from the login manager.

| Option | Type | Description |
|--------|------|-------------|
| **openbox** | Stacking | Traditional, highly configurable, extensive theme support |
| **fluxbox** | Stacking | Similar to openbox, adds window tabbing |
| **icewm** | Stacking | Very fast, familiar Windows-like feel |
| **i3** | Tiling | Keyboard-driven, windows arranged in a grid |

**Stacking** = windows overlap (traditional). **Tiling** = windows never overlap, auto-arranged.

### Compositor
Adds visual effects like shadows, transparency, and fade animations. Runs alongside the window manager to composite windows before displaying them.

**Current choice:** picom

### Panel
Provides the taskbar, system tray, clock, and workspace switcher. The main persistent UI element on screen.

**Current choice:** tint2

### Application Menu
Launches applications. Can be triggered by clicking or keyboard shortcut.

**Current choice:** jgmenu

### Notification Daemon
Displays desktop notifications (the popups for messages, alerts, etc.).

**Current choice:** dunst

### Wallpaper Manager
Sets and restores the desktop background image.

**Current choice:** nitrogen

### File Manager
Browses files and handles automounting of removable drives.

**Current choice:** pcmanfm

### Authentication Agent
Handles password prompts when applications need elevated privileges (sudo/admin actions).

**Current choice:** lxpolkit

### Systray Applets
Small indicator apps that live in the system tray for quick access to system functions.

**Current choices:**
- **pasystray** - volume control
- **nm-applet** - network/wifi management
- **blueman-applet** - bluetooth (optional)

## Installation

### 1. Install dependencies

```bash
# Core components
sudo apt install picom dunst nitrogen pcmanfm lxpolkit pasystray network-manager-gnome dex tint2 jgmenu

# Window managers (install all to switch between them)
sudo apt install openbox fluxbox icewm i3
```

### 2. Install OpenDE

```bash
# Clone or download the repository, then:
sudo ./install.sh
```

This installs OpenDE to `/usr/local/share/opende/` and registers the sessions with your display manager.

### Uninstall

```bash
sudo ./uninstall.sh
```

## Structure

- `config/` - configuration files for each component
- `scripts/` - helper scripts
- `sessions/` - X session files for login manager

## Usage

Select one of the OpenDE sessions from your login manager:
- **OpenDE (Openbox)** - traditional stacking
- **OpenDE (Fluxbox)** - stacking with tabbed windows
- **OpenDE (IceWM)** - lightweight stacking
- **OpenDE (i3)** - tiling, keyboard-driven

## Customization

| Component | How to Customize | Config File |
|-----------|------------------|-------------|
| **Wallpaper** | Run `nitrogen` | `~/.config/nitrogen/` |
| **Openbox theme/fonts** | Run `obconf` or `lxappearance` | `~/.config/openbox/rc.xml` |
| **Keyboard shortcuts** | Edit rc.xml `<keyboard>` section | `~/.config/openbox/rc.xml` |
| **Openbox right-click menu** | Edit menu.xml | `~/.config/openbox/menu.xml` |
| **Compositor effects** | Edit picom.conf | `~/.config/picom/picom.conf` |
| **Notification style** | Edit dunstrc | `~/.config/dunst/dunstrc` |
| **Panel tweaks** | Run `tint2conf` or edit tint2rc | `~/.config/tint2/tint2rc` |
| **Application menu** | Edit jgmenurc and append.csv | `~/.config/jgmenu/jgmenurc` |

## Keyboard Shortcuts

Common shortcuts (configured consistently across all WMs where possible):

| Shortcut | Action |
|----------|--------|
| `Alt+Tab` | Switch windows |
| `Alt+F4` | Close window |
| `Super+D` | Show desktop |
| `Super` | Application menu (jgmenu) |

**Note:** i3 uses additional keybindings for tiling - see `config/i3/config`.

## CLI Tool

OpenDE includes a command-line tool for managing settings:

```bash
# Interactive mode
opende config

# Show all settings
opende status

# Manage specific categories
opende effects status
opende effects enable shadows
opende effects set transparency 85

opende panel set position bottom
opende panel enable autohide

opende input enable natural-scrolling    # requires sudo
opende input set mouse-accel medium      # requires sudo
```

### Available Settings

| Category | Setting | Values |
|----------|---------|--------|
| effects | compositor | enable/disable |
| effects | shadows | enable/disable |
| effects | animations | enable/disable |
| effects | transparency | 0-100 |
| panel | position | top/bottom |
| panel | autohide | enable/disable |
| input | natural-scrolling | enable/disable (sudo) |
| input | tap-to-click | enable/disable (sudo) |
| input | mouse-accel | off/low/medium/high (sudo) |

## Useful Commands

```bash
# Reload panel
killall -SIGUSR1 tint2

# Test notifications
notify-send "Title" "Message"

# Set wallpaper
nitrogen

# Reload window manager config
openbox --reconfigure     # Openbox
fluxbox-remote restart    # Fluxbox
icewm --restart           # IceWM
i3-msg reload             # i3
```
