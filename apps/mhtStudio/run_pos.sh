#!/usr/bin/env bash
set -euo pipefail

APP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

PY="$APP_DIR/.venv/bin/python"
BASHRC="$APP_DIR/../../etc/bashrc"
LOG="$APP_DIR/run_pos.log"

# Sanity checks
if [ ! -x "$PY" ]; then
  echo "[ERROR] venv python not found: $PY"
  echo "Run ./install.sh first."
  exit 1
fi

if [ -f "$BASHRC" ]; then
  # shellcheck disable=SC1090
  source "$BASHRC"
else
  echo "[WARNING] bashrc not found: $BASHRC"
fi

if pgrep -f "$APP_DIR/main_pos.py" >/dev/null; then
  echo "[ERROR] main_pos.py already running. Kill it first:"
  echo "  pkill -f '$APP_DIR/main_pos.py'"
  exit 1
fi


# Run in background and log everything

nohup "$PY" -u "$APP_DIR/main_pos.py" >"$LOG" 2>&1 &

PID=$!
echo "[OK] mhtStudio post-processing started (PID=$PID)"
echo "[OK] Log: $LOG"
