#!/usr/bin/env bash
set -e

APP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Source fhdFoam environment
if [ -f "$APP_DIR/../../etc/bashrc" ]; then
  source "$APP_DIR/../../etc/bashrc"
fi

# Run using the local venv python
exec "$APP_DIR/.venv/bin/python" "$APP_DIR/main_pre.py"
