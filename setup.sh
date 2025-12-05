#!/bin/bash
# QWERTY MIDI Keyboard - Setup Script
# Installs dependencies and sets up auto-launch for macOS and Linux

set -e

echo "QWERTY MIDI Keyboard - Setup"
echo "================================"
echo ""

# Detect OS
OS="$(uname -s)"
case "${OS}" in
    Linux*)     MACHINE=Linux;;
    Darwin*)    MACHINE=Mac;;
    *)          MACHINE="UNKNOWN:${OS}"
esac

echo "Detected OS: $MACHINE"
echo ""

# Check Python
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 is required but not installed."
    echo "Please install Python 3 and try again."
    exit 1
fi

PYTHON_VERSION=$(python3 --version | cut -d' ' -f2)
echo "Python $PYTHON_VERSION found"
echo ""

# Setup Python environment
echo "Setting up Python environment..."
cd midi_sound_engine

if [ ! -d "venv" ]; then
    python3 -m venv venv
    echo "Created virtual environment"
else
    echo "Virtual environment already exists"
fi

source venv/bin/activate
pip install --upgrade pip > /dev/null 2>&1
pip install -r requirements.txt

# Install macOS-specific dependencies if on macOS
if [ "$MACHINE" = "Mac" ]; then
    if [ -f "requirements-macos.txt" ]; then
        pip install -r requirements-macos.txt
    fi
fi

echo "Python dependencies installed"
echo ""

# Setup firmware build (if CMake available)
cd ../qwerty_midi_pico
if command -v cmake &> /dev/null; then
    echo "Setting up firmware build environment..."
    if [ ! -d "build" ]; then
        mkdir build
    fi
    echo "Firmware build environment ready"
    echo "   Run 'cd qwerty_midi_pico/build && cmake .. && make' to build"
else
    echo "WARNING: CMake not found - firmware build skipped"
    echo "   Install CMake to build firmware"
fi
echo ""

# Setup auto-launch
cd ..
echo "Setting up auto-launch..."

if [ "$MACHINE" = "Mac" ]; then
    # macOS LaunchAgent
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    LAUNCH_AGENT_DIR="$HOME/Library/LaunchAgents"
    PLIST_FILE="$LAUNCH_AGENT_DIR/com.qwertymidi.pico.plist"
    
    mkdir -p "$LAUNCH_AGENT_DIR"
    
    cat > "$PLIST_FILE" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.qwertymidi.pico</string>
    <key>ProgramArguments</key>
    <array>
        <string>$SCRIPT_DIR/midi_sound_engine/venv/bin/python3</string>
        <string>$SCRIPT_DIR/midi_sound_engine/monitor_and_launch.py</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
    <key>StandardOutPath</key>
    <string>$HOME/Library/Logs/qwerty_midi.log</string>
    <key>StandardErrorPath</key>
    <string>$HOME/Library/Logs/qwerty_midi_error.log</string>
</dict>
</plist>
EOF
    
    # Load the launch agent
    launchctl unload "$PLIST_FILE" 2>/dev/null || true
    launchctl load "$PLIST_FILE"
    
    echo "macOS LaunchAgent installed"
    echo "   Logs: ~/Library/Logs/qwerty_midi.log"
    
elif [ "$MACHINE" = "Linux" ]; then
    # Linux systemd user service
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    SYSTEMD_DIR="$HOME/.config/systemd/user"
    
    mkdir -p "$SYSTEMD_DIR"
    
    cat > "$SYSTEMD_DIR/qwerty-midi.service" << EOF
[Unit]
Description=QWERTY MIDI Keyboard Monitor
After=network.target

[Service]
Type=simple
ExecStart=$SCRIPT_DIR/midi_sound_engine/venv/bin/python3 $SCRIPT_DIR/midi_sound_engine/monitor_and_launch.py
Restart=always
RestartSec=5

[Install]
WantedBy=default.target
EOF
    
    systemctl --user daemon-reload
    systemctl --user enable qwerty-midi.service
    systemctl --user start qwerty-midi.service
    
    echo "Linux systemd service installed"
    echo "   Manage with: systemctl --user [start|stop|status] qwerty-midi.service"
else
    echo "WARNING: Auto-launch not configured for $MACHINE"
    echo "   Please set up manually or use the monitor script"
fi
echo ""

echo "Setup complete!"
echo ""
echo "Next steps:"
echo "  1. Flash firmware to Pico (see qwerty_midi_pico/README.md)"
echo "  2. Plug in your Pico"
echo "  3. The synthesizer will auto-launch!"
echo ""
echo "Manual start:"
echo "  cd midi_sound_engine && source venv/bin/activate && python monitor_and_launch.py"

