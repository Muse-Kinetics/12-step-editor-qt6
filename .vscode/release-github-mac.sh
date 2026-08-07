#!/usr/bin/env bash
# release-github-mac.sh — upload the built macOS .dmg to the GitHub Release
# whose tag matches the editor's current version (v<CFBundleShortVersionString>).
#
# Usage: release-github-mac.sh <owner/repo> <volume_name> <App.app path>
#   The App is only read for its version; the DMG that "Mac Make DMG" produced
#   at <dist>/<volume> Mac v<version>.dmg is what gets uploaded.
#
# Env:
#   KMI_DIST_DIR        dir holding the DMG (default: <workspace>/dist-macos)
#   KMI_CREATE_RELEASE=1  create the release if the tag doesn't exist yet
set -euo pipefail

REPO="${1:?owner/repo required}"
VOLUME="${2:?volume name required}"
APP="${3:?App.app path (for version) required}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
WORKSPACE="$(cd "$SCRIPT_DIR/.." >/dev/null 2>&1 && pwd)"
DIST="${KMI_DIST_DIR:-$WORKSPACE/dist-macos}"

command -v gh >/dev/null || { echo "ERROR: gh CLI not installed" >&2; exit 1; }
[[ -d "$APP" ]] || { echo "ERROR: app not found: $APP (run 'Mac Make DMG' first)" >&2; exit 1; }
# Version from KMI_VERSION > VERSION= in KMI_VERSION_PRO (.pro) > Info.plist —
# must match what "Mac Make DMG" used so the DMG filename resolves.
if [[ -n "${KMI_VERSION:-}" ]]; then
    VERSION="$KMI_VERSION"
elif [[ -n "${KMI_VERSION_PRO:-}" && -f "${KMI_VERSION_PRO}" ]]; then
    VERSION="$(grep -E '^VERSION[[:space:]]*=' "$KMI_VERSION_PRO" | head -1 | sed -E 's/^VERSION[[:space:]]*=[[:space:]]*//' | tr -d '[:space:]')"
else
    VERSION="$(/usr/libexec/PlistBuddy -c 'Print :CFBundleShortVersionString' "$APP/Contents/Info.plist")"
fi
TAG="v$VERSION"
DMG="$DIST/$VOLUME Mac v$VERSION.dmg"
[[ -f "$DMG" ]] || { echo "ERROR: DMG not found: $DMG — run the 'Mac Make DMG' task first." >&2; exit 1; }

echo "Repo: $REPO   Tag: $TAG"
echo "DMG:  $DMG"

if gh release view "$TAG" -R "$REPO" >/dev/null 2>&1; then
    echo "Uploading DMG to existing release $TAG ..."
    gh release upload "$TAG" "$DMG" --clobber -R "$REPO"
elif [[ "${KMI_CREATE_RELEASE:-0}" == "1" ]]; then
    echo "Release $TAG not found — creating it (KMI_CREATE_RELEASE=1) ..."
    NOTES="macOS universal build — $VOLUME v$VERSION"
    gh release create "$TAG" "$DMG" -R "$REPO" --title "$VOLUME v$VERSION" --notes "$NOTES"
else
    echo "ERROR: release $TAG does not exist on $REPO." >&2
    echo "The matching (Windows) release should already be published. To create it" >&2
    echo "from here anyway, re-run with KMI_CREATE_RELEASE=1." >&2
    exit 1
fi

echo "Done. Assets now on $REPO $TAG:"
gh release view "$TAG" -R "$REPO" --json assets --jq '.assets[].name' | sed 's/^/  - /'
