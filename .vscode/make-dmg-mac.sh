#!/usr/bin/env bash
# make-dmg-mac.sh — build a signed, notarized, stapled .dmg for one or more
# KMI editor .apps (SoftStep ships two apps in a single DMG).
#
# Usage: make-dmg-mac.sh <volume_name> <App1.app> [App2.app ...]
#   volume_name  DMG volume + output basename ("<volume> Mac v<version>.dmg")
#
# Env overrides:
#   KMI_QMLDIR         -qmldir passed to macdeployqt (editors that use QML)
#   KMI_NOTARY_PROFILE notarytool keychain profile   (default: Andrej)
#   KMI_SIGN_DIR       dir with entitlements.mac.plist + <name>.icns + dmg/
#   KMI_DIST_DIR       output dir (default: <workspace>/dist-macos)
#   KMI_SKIP_NOTARIZE=1  sign + build dmg but skip notarize/staple
set -euo pipefail

VOLUME="${1:?volume name required}"; shift
APPS=("$@")
[[ ${#APPS[@]} -ge 1 ]] || { echo "ERROR: at least one .app path required" >&2; exit 1; }

IDENTITY="Developer ID Application: Kesumo, LLC (J372N6RANB)"
NOTARY_PROFILE="${KMI_NOTARY_PROFILE:-Andrej}"
QTBIN="${QTBIN:-$HOME/Qt/6.9.2/macos/bin}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
WORKSPACE="$(cd "$SCRIPT_DIR/.." >/dev/null 2>&1 && pwd)"
SIGN_DIR="${KMI_SIGN_DIR:-$WORKSPACE/Signing and Notarization}"
DIST="${KMI_DIST_DIR:-$WORKSPACE/dist-macos}"
ENT="$SIGN_DIR/entitlements.mac.plist"
QMLDIR="${KMI_QMLDIR:-}"

command -v create-dmg >/dev/null || { echo "ERROR: create-dmg not installed (brew install create-dmg)" >&2; exit 1; }
[[ -x "$QTBIN/macdeployqt" ]] || { echo "ERROR: macdeployqt not found at $QTBIN" >&2; exit 1; }

# Version: KMI_VERSION > VERSION= from KMI_VERSION_PRO (the .pro, single source
# of truth, same as the Windows release-github.ps1) > app Info.plist.
resolve_version() {
    if [[ -n "${KMI_VERSION:-}" ]]; then echo "$KMI_VERSION"; return; fi
    if [[ -n "${KMI_VERSION_PRO:-}" && -f "${KMI_VERSION_PRO}" ]]; then
        local v; v="$(grep -E '^VERSION[[:space:]]*=' "$KMI_VERSION_PRO" | head -1 | sed -E 's/^VERSION[[:space:]]*=[[:space:]]*//' | tr -d '[:space:]')"
        [[ -n "$v" ]] && { echo "$v"; return; }
    fi
    /usr/libexec/PlistBuddy -c 'Print :CFBundleShortVersionString' "$1/Contents/Info.plist" 2>/dev/null || echo 0.0.0
}
VERSION="$(resolve_version "${APPS[0]}")"
mkdir -p "$DIST"; xattr -w com.dropbox.ignored 1 "$DIST" 2>/dev/null || true
OUT_DMG="$DIST/$VOLUME Mac v$VERSION.dmg"

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

for APP in "${APPS[@]}"; do
    [[ -d "$APP" ]] || { echo "ERROR: app not found: $APP (run the Mac Build task first)" >&2; exit 1; }
    NAME="$(basename "$APP" .app)"
    echo "=== macdeployqt + codesign: $NAME v$VERSION ==="
    dq=( "$APP" -verbose=1 "-codesign=$IDENTITY" "-executable=$APP/Contents/MacOS/$NAME" )
    [[ -n "$QMLDIR" ]] && dq+=( "-qmldir=$QMLDIR" )
    "$QTBIN/macdeployqt" "${dq[@]}"
    # stamp the resolved version into the bundle (qmake doesn't reliably set it)
    plutil -replace CFBundleShortVersionString -string "$VERSION" "$APP/Contents/Info.plist" 2>/dev/null || true
    plutil -replace CFBundleVersion -string "$VERSION" "$APP/Contents/Info.plist" 2>/dev/null || true
    cs=( -s "$IDENTITY" --options runtime --timestamp --force --deep )
    if [[ -f "$ENT" ]]; then cs+=( --entitlements "$ENT" ); else echo "WARN: no entitlements at $ENT — signing without them"; fi
    codesign "${cs[@]}" "$APP"
done

# volume/folder icon: prefer <volume>.icns, else the first app's <name>.icns
ICNS="$SIGN_DIR/$VOLUME.icns"
[[ -f "$ICNS" ]] || ICNS="$SIGN_DIR/$(basename "${APPS[0]}" .app).icns"

# Stage + lay out to match the legacy KMI DMGs (window 530x380, drag to
# Applications on the right). If the editor bundles a Content/ folder (all four
# Qt editors do) or ships more than one app (SoftStep), everything is grouped
# inside a single "<Volume>" folder — app(s) + CHANGELOG.md + Content/ — with a
# custom folder icon, exactly as the legacy sign/notarize scripts did. An editor
# with no content and one app (K-Mix) ships a loose app icon instead.
CONTENT="${KMI_DMG_CONTENT:-$WORKSPACE/Content}"
CHANGELOG="${KMI_DMG_CHANGELOG:-$WORKSPACE/CHANGELOG.md}"
N=${#APPS[@]}
USE_FOLDER=0
[[ $N -gt 1 || -d "$CONTENT" ]] && USE_FOLDER=1

if [[ $USE_FOLDER -eq 1 ]]; then
    mkdir -p "$STAGE/$VOLUME"
    for APP in "${APPS[@]}"; do cp -R "$APP" "$STAGE/$VOLUME/"; done
    [[ -f "$CHANGELOG" ]] && cp -R "$CHANGELOG" "$STAGE/$VOLUME/" && echo "  bundled CHANGELOG.md"
    if [[ -d "$CONTENT" ]]; then cp -R "$CONTENT" "$STAGE/$VOLUME/" && echo "  bundled Content/ ($(du -sh "$CONTENT" | cut -f1))"; fi
    [[ -f "$ICNS" ]] && command -v fileicon >/dev/null && fileicon set "$STAGE/$VOLUME" "$ICNS" >/dev/null 2>&1 || true
    ITEM="$VOLUME"; IX=160; ISIZE=100; DROP_X=375
else
    cp -R "${APPS[0]}" "$STAGE/"
    ITEM="$(basename "${APPS[0]}")"; IX=155; ISIZE=90; DROP_X=370
fi

echo "=== create-dmg: $OUT_DMG ==="
[[ -f "$OUT_DMG" ]] && rm -f "$OUT_DMG"
cd_args=( --volname "$VOLUME" --window-pos 200 120 --window-size 530 380
          --icon-size "$ISIZE" --icon "$ITEM" "$IX" 220 --app-drop-link "$DROP_X" 220 )
[[ -f "$ICNS" ]] && cd_args+=( --volicon "$ICNS" )
BG="$(ls "$SIGN_DIR"/dmg/background*.tiff "$SIGN_DIR"/dmg/background*.png 2>/dev/null | head -1 || true)"
[[ -n "$BG" ]] && cd_args+=( --background "$BG" )
create-dmg "${cd_args[@]}" "$OUT_DMG" "$STAGE/" \
  || create-dmg --volname "$VOLUME" "$OUT_DMG" "$STAGE/"

echo "=== codesign dmg ==="
xattr -cr "$OUT_DMG"
codesign -s "$IDENTITY" --options runtime --timestamp --force "$OUT_DMG"

if [[ "${KMI_SKIP_NOTARIZE:-0}" == "1" ]]; then
    echo "KMI_SKIP_NOTARIZE=1 — built signed (un-notarized) DMG: $OUT_DMG"; exit 0
fi
echo "=== notarize (profile: $NOTARY_PROFILE) ==="
xcrun notarytool submit "$OUT_DMG" --keychain-profile "$NOTARY_PROFILE" --wait
echo "=== staple ==="
xcrun stapler staple "$OUT_DMG"
xcrun stapler validate "$OUT_DMG"
echo "DMG ready: $OUT_DMG"
