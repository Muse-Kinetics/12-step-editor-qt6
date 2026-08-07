#!/usr/bin/env bash
# launch-on-console.sh — run a built KMI editor directly so all qDebug/stderr
# output shows in the calling terminal (the VS Code integrated terminal).
#
# Usage: launch-on-console.sh <path/to/App.app/Contents/MacOS/App>

set -euo pipefail
PROGRAM="${1:?Usage: launch-on-console.sh <path to app binary>}"

if [[ ! -x "$PROGRAM" ]]; then
    echo "ERROR: not found or not executable: $PROGRAM" >&2
    echo "Build it first (the matching 'Mac Build' task)." >&2
    exit 1
fi

# Fallback framework path in case the binary is run outside its build dir.
for c in "$HOME/Qt/6.9.2/macos/lib" "$HOME/Qt/6.9.1/macos/lib"; do
    [[ -d "$c" ]] && { export DYLD_FRAMEWORK_PATH="${c}${DYLD_FRAMEWORK_PATH:+:$DYLD_FRAMEWORK_PATH}"; break; }
done

exec "$PROGRAM"
