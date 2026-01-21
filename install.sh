#!/bin/bash
# OpenDE Installer
# Installs OpenDE to standard system locations
# Run as: sudo ./install.sh

set -e

# Installation paths
INSTALL_DIR="/usr/local/share/opende"
XSESSIONS_DIR="/usr/share/xsessions"

# Get the directory where this script lives (source of files to install)
SOURCE_DIR="$(cd "$(dirname "$0")" && pwd)"

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
    error "Please run as root: sudo ./install.sh"
fi

# Check that we're running from the source directory
if [ ! -d "$SOURCE_DIR/scripts" ] || [ ! -d "$SOURCE_DIR/config" ]; then
    error "Cannot find scripts/ and config/ directories. Run this from the OpenDE source directory."
fi

info "Installing OpenDE to $INSTALL_DIR"

# Create installation directory
mkdir -p "$INSTALL_DIR"

# Copy scripts
info "Installing scripts..."
mkdir -p "$INSTALL_DIR/scripts"
cp "$SOURCE_DIR/scripts/"* "$INSTALL_DIR/scripts/"
chmod +x "$INSTALL_DIR/scripts/"*

# Copy configs
info "Installing configurations..."
cp -r "$SOURCE_DIR/config" "$INSTALL_DIR/"

# Copy systemd user service
info "Installing systemd user service..."
mkdir -p "$INSTALL_DIR/systemd/user"
cp "$SOURCE_DIR/systemd/user/tint2.service" "$INSTALL_DIR/systemd/user/"

# Generate and install .desktop session files
info "Installing session files to $XSESSIONS_DIR..."
mkdir -p "$XSESSIONS_DIR"

for desktop_file in "$SOURCE_DIR/sessions/"*.desktop; do
    filename=$(basename "$desktop_file")
    # Replace the hardcoded path with the install location
    sed "s|Exec=.*/scripts/|Exec=$INSTALL_DIR/scripts/|" \
        "$desktop_file" > "$XSESSIONS_DIR/$filename"
done

info "Installation complete!"
echo ""
echo "Next steps:"
echo "  1. Install dependencies (if not already installed):"
echo "     sudo apt install openbox picom dunst nitrogen pcmanfm lxpolkit \\"
echo "                      pasystray network-manager-gnome dex tint2 jgmenu"
echo ""
echo "  2. Log out and select 'OpenDE (Openbox)' or another OpenDE session"
echo "     from your display manager"
echo ""
echo "To uninstall: sudo ./uninstall.sh (from the source directory)"
