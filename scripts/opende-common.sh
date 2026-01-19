#!/bin/bash
# OpenDE Common Startup - Source this from WM-specific session scripts
# Usage: source opende-common.sh

# Determine OpenDE install directory (parent of scripts/)
OPENDE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Export desktop session info
export XDG_CURRENT_DESKTOP=OpenDE
export XDG_SESSION_DESKTOP=opende
export OPENDE_DIR

# Ensure local bin is in PATH
export PATH="/usr/local/bin:$PATH"

# Load X resources if present
[ -f ~/.Xresources ] && xrdb -merge ~/.Xresources

# Start compositor (transparency, shadows)
picom -b --config "$OPENDE_DIR/config/picom.conf" &
sleep 0.5

# Set wallpaper (nitrogen remembers last setting)
nitrogen --restore &

# Start notification daemon
dunst -config "$OPENDE_DIR/config/dunstrc" &

# Start polkit agent (for password prompts)
lxpolkit &

# Start systray apps
nm-applet &                    # Network
pasystray &                    # Volume
blueman-applet &               # Bluetooth

# Start file manager daemon (handles USB automounting)
pcmanfm -d &

# Give systray apps time to start before panel
sleep 1

# Start panel via systemd (enables crash recovery and logging)
# Ensure service is installed
SYSTEMD_USER_DIR="$HOME/.config/systemd/user"
mkdir -p "$SYSTEMD_USER_DIR"
if [ ! -e "$SYSTEMD_USER_DIR/tint2.service" ]; then
    ln -sf "$OPENDE_DIR/systemd/user/tint2.service" "$SYSTEMD_USER_DIR/tint2.service"
    systemctl --user daemon-reload
fi
systemctl --user start tint2

# Run any .desktop autostart files
dex -a -s ~/.config/autostart/ 2>/dev/null &

# Small delay to let everything settle before WM starts
sleep 0.5
