#!/usr/bin/env bash
set -euo pipefail

echo ""
echo "╔══════════════════════════════════════════════════════════════════════════════╗"
echo "║ 🧲  mhtStudio — Installer                                                    ║"
echo "║ 📦  Creates a local venv (system Python) and installs dependencies           ║"
echo "╚══════════════════════════════════════════════════════════════════════════════╝"
echo ""

# Resolve the script directory (mhtStudio folder)
APP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$APP_DIR"

VENV_DIR="${VENV_DIR:-.venv}"

# Prefer system Python to avoid Conda/Anaconda Tk issues (pixelated widgets)
if [ -x /usr/bin/python3 ]; then
  PY_BIN_DEFAULT="/usr/bin/python3"
else
  PY_BIN_DEFAULT="python3"
fi
PY_BIN="${PY_BIN:-$PY_BIN_DEFAULT}"

if ! command -v "$PY_BIN" >/dev/null 2>&1; then
  echo "[ERROR] '$PY_BIN' not found. Please install Python 3 first."
  exit 1
fi

echo "[INFO] Requested Python: $PY_BIN"
echo "[INFO] Python version  : $("$PY_BIN" --version)"
echo "[INFO] App folder      : $APP_DIR"

# Guard rail: refuse to create venv with Conda/Anaconda Python
PY_INFO="$("$PY_BIN" - <<'PY'
import sys
print(sys.version)
PY
)"

if echo "$PY_INFO" | grep -qiE 'conda|anaconda'; then
  echo ""
  echo "[ERROR] Refusing to create venv with a Conda/Anaconda Python."
  echo "        This causes Tk/Tcl rendering issues (pixelated/quadriculated UI)."
  echo ""
  echo "Fix: run with system Python:"
  echo "  PY_BIN=/usr/bin/python3 ./install.sh"
  exit 1
fi


# If venv exists, verify its Python is NOT Conda-based and is a stable version
NEED_RECREATE="false"
if [ -d "$VENV_DIR" ]; then
  if [ ! -x "$VENV_DIR/bin/python" ]; then
    NEED_RECREATE="true"
  else
    VENV_PY_VER="$("$VENV_DIR/bin/python" --version 2>/dev/null || true)"
    VENV_PY_INFO="$("$VENV_DIR/bin/python" - <<'PY' 2>/dev/null || true
import sys
print(sys.version)
PY
)"
    if echo "$VENV_PY_INFO" | grep -qiE 'conda|anaconda'; then
      echo "[WARNING] Existing venv uses Conda/Anaconda Python. Recreating venv..."
      NEED_RECREATE="true"
    fi
    if echo "$VENV_PY_VER" | grep -q "Python 3.13"; then
      echo "[WARNING] Existing venv is Python 3.13. Recreating venv with system Python..."
      NEED_RECREATE="true"
    fi
  fi
fi

if [ "$NEED_RECREATE" = "true" ]; then
  rm -rf "$VENV_DIR"
fi

if [ ! -d "$VENV_DIR" ]; then
  echo "[INFO] Creating virtual environment in: $APP_DIR/$VENV_DIR"
  "$PY_BIN" -m venv "$VENV_DIR"
else
  echo "[INFO] Virtual environment already exists: $APP_DIR/$VENV_DIR"
fi

# Activate venv
# shellcheck disable=SC1091
source "$VENV_DIR/bin/activate"

echo "[INFO] Active python    : $(which python)"
echo "[INFO] Active version   : $(python --version)"

echo "[INFO] Upgrading pip tooling..."
python -m pip install --upgrade pip setuptools wheel

DEPS=(
  "customtkinter"
  "matplotlib"
  "pillow"
  "numpy"
  "pyqt6"
)

echo "[INFO] Installing dependencies:"
printf "  - %s\n" "${DEPS[@]}"
python -m pip install "${DEPS[@]}"

# -----------------------------------------------------------------------------
# Source fhdFoam project environment (for this shell session)
# -----------------------------------------------------------------------------
FHD_BASHRC="$APP_DIR/../../etc/bashrc"
if [ -f "$FHD_BASHRC" ]; then
  echo "[INFO] Sourcing fhdFoam environment: $FHD_BASHRC"
  # shellcheck disable=SC1090
  source "$FHD_BASHRC"
else
  echo "[WARNING] fhdFoam/etc/bashrc not found: $FHD_BASHRC"
  echo "          Some environment variables may be missing."
fi

echo ""
echo "╔══════════════════════════════════════════════════════════════════════════════╗"
echo "║ ✅  Installation finished                                                    ║"
echo "╚══════════════════════════════════════════════════════════════════════════════╝"
echo ""
echo " Run mhtStudio (in this same terminal):"
echo "   cd $APP_DIR"
echo "   chmod +x run.sh"
echo "   ./run.sh"
echo ""
echo " If you open a new terminal, remember:"
echo "   source $APP_DIR/$VENV_DIR/bin/activate"
echo "   source $FHD_BASHRC"

echo ""

# Quick import check
echo "[INFO] Quick import check..."
python - <<'PY'
import importlib
mods = ["customtkinter", "matplotlib", "PIL", "numpy"]
for m in mods:
    importlib.import_module(m)
    print(f"[OK] {m}")
print("[OK] All imports succeeded.")
PY

