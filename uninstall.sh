#!/bin/bash
# OpenDE Uninstaller
# Removes OpenDE from system locations
# Run as: sudo ./uninstall.sh

set -e

# Installation paths (must match install.sh)
INSTALL_DIR="/usr/local/share/opende"
XSESSIONS_DIR="/usr/share/xsessions"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

info() { echo -e "${GREEN}[INFO]${NC} $1"; }
warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
error() { echo -e "${RED}[ERROR]${NC} $1"; exit 1; }

# Check for root privileges
if [ "$EUID" -ne 0 ]; then
    error "Please run as root: sudo ./uninstall.sh"
fi

info "Uninstalling OpenDE..."

# Remove installation directory
if [ -d "$INSTALL_DIR" ]; then
    info "Removing $INSTALL_DIR..."
    rm -rf "$INSTALL_DIR"
else
    warn "$INSTALL_DIR not found, skipping"
fi

# Remove session files
info "Removing session files from $XSESSIONS_DIR..."
rm -f "$XSESSIONS_DIR/opende-openbox.desktop"
rm -f "$XSESSIONS_DIR/opende-fluxbox.desktop"
rm -f "$XSESSIONS_DIR/opende-i3.desktop"
rm -f "$XSESSIONS_DIR/opende-icewm.desktop"

info "Uninstallation complete!"
echo ""
echo "Note: The following were NOT removed:"
echo "  - User configurations in ~/.config/"
echo "  - Installed apt packages (openbox, picom, etc.)"
echo "  - systemd user service symlinks in ~/.config/systemd/user/"
echo ""
echo "To remove user service symlinks, run (as your normal user):"
echo "  rm -f ~/.config/systemd/user/tint2.service"
echo "  systemctl --user daemon-reload"
