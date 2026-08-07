#!/usr/bin/env bash
# run-qmake-task.sh — macOS build helper for the KMI Qt editors.
#
# Usage: run-qmake-task.sh <project.pro> <configure|build|clean> [build_dir] [extra qmake args...]
#
# Override the qmake used with the QMAKE environment variable.

set -euo pipefail

PROJECT_FILE="${1:?project file required}"
ACTION="${2:?action required (configure|build|clean)}"
BUILD_DIR="${3:-}"
shift 3 2>/dev/null || shift $#
CONFIG_ARGS=("$@")   # e.g. CONFIG+=release

# ── Locate qmake ────────────────────────────────────────────────────────────
if [[ -n "${QMAKE:-}" ]]; then
    QMAKE_CMD="$QMAKE"
else
    for c in "$HOME/Qt/6.9.2/macos/bin/qmake" "$HOME/Qt/6.9.1/macos/bin/qmake" \
             "/opt/homebrew/opt/qt/bin/qmake" "/usr/local/opt/qt/bin/qmake"; do
        [[ -x "$c" ]] && { QMAKE_CMD="$c"; break; }
    done
    : "${QMAKE_CMD:=$(command -v qmake 2>/dev/null || true)}"
fi
[[ -n "${QMAKE_CMD:-}" ]] || { echo "ERROR: qmake not found. Set the QMAKE env var." >&2; exit 1; }

# ── macOS build fix (Qt 6.9.x + Apple clang) ────────────────────────────────
# Qt 6.9.2's qyieldcpu.h does `#if __has_builtin(__yield) -> __yield();`, and
# Apple clang (Xcode 16+) treats the implicit __yield declaration as a hard
# error. The Windows editors build on Qt 6.3.2 and never hit this. Downgrade
# it so every editor compiles on this machine's Qt 6.9.2.
MAC_FIX=( "QMAKE_CXXFLAGS+=-Wno-error=implicit-function-declaration"
          "QMAKE_CFLAGS+=-Wno-error=implicit-function-declaration" )

# ── Build dir (kept out of Dropbox's sync) ──────────────────────────────────
if [[ -n "$BUILD_DIR" ]]; then
    mkdir -p "$BUILD_DIR"
    xattr -w com.dropbox.ignored 1 "$BUILD_DIR" 2>/dev/null || true
    cd "$BUILD_DIR"
fi

CPU="$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"

case "$ACTION" in
    configure)
        "$QMAKE_CMD" "$PROJECT_FILE" "${MAC_FIX[@]}" ${CONFIG_ARGS[@]+"${CONFIG_ARGS[@]}"}
        ;;
    build)
        "$QMAKE_CMD" "$PROJECT_FILE" "${MAC_FIX[@]}" ${CONFIG_ARGS[@]+"${CONFIG_ARGS[@]}"}
        make -j"$CPU"
        ;;
    clean)
        if [[ -f Makefile ]]; then make clean; else echo "No Makefile in $(pwd); nothing to clean."; fi
        ;;
    *)
        echo "ERROR: unsupported action: $ACTION" >&2; exit 2 ;;
esac
