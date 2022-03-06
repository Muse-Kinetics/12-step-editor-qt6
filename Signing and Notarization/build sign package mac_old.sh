#!/bin/bash

# This script packages the K-Mix mac application and handles the Apple signing/notarization process.
# - You must use the entitlements included in this directory.

# app_name is the name of the dmg volume
app_name="12 Step"
app_name_fp=12\ Step
version=2.0.1
bundle_id="com.keithmcmillen.12 Step Editor"
app_path=./dmg/12\ Step\ Editor\ Mac\ v2.0/12\ Step\ Editor.app/
package_path=./dmg/

#libusb_path=$app_path/Contents/Resources/libusb-1.0.0.dylib
#xmosdfu_path=$app_path/Contents/Resources/xmosdfu
#editor_path=$app_path/Contents/MacOS/K-Mix\ Editor

temp_dmg_name=./12\ Step\ Editor\ Mac\ v$version.p.dmg
final_dmg_name=./12\ Step\ Editor\ Mac\ v$version.dmg


# set current directory to where the script was called from
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

echo "$app_name Builder v$version"
echo "$temp_dmg_name"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

# clean dmg attributes
xattr -cr "$package_path"


# sign the code
# 12 step needs QT frameworks plists copied. See: http://barenka.blogspot.com/2014/04/signing-qt5-520-applications-in-mac-osx.html
codesign -s "Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$app_path"

# sign libusb

#codesign -s "Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$libusb_path"

# sign xmosdfu

#codesign -s "Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$xmosdfu_path"

# sign k-mix editor

#codesign -s "Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$editor_path"


# create dmg
hdiutil create "$temp_dmg_name" -volname "$app_name" -format UDRW -fs HFS -srcfolder "$package_path"

# mount image to adjust background etc

echo "..."
echo "Pressing enter will open the temporary DMG."
echo "1) Organize the icons, 2) set the background as needed, and 3) then close the window."
echo
read -n 1 -s -r -p ""
open "$temp_dmg_name"
sleep 4 # wait for image to mount
open /Volumes/"$app_name_fp"

echo "..."
echo "Once the volume is ejected, press enter again and it will be re-opened."
echo "Verify that everything looks correct."
echo
read -n 1 -s -r -p ""

# eject
hdiutil detach /Volumes/$app_name_fp/
open "$temp_dmg_name"
sleep 4 # wait for image to mount
open /Volumes/"$app_name_fp"


echo "If everything looks correct, eject the volume and press enter to finalize."
echo
read -n 1 -s -r -p ""

# convert to read only
hdiutil convert -format UDZO -o "$final_dmg_name" "$temp_dmg_name"
rm -rf "$temp_dmg_name"

# clean dmg attributes
xattr -cr "$final_dmg_name"

echo "signing...."
echo

# sign the dmg
codesign -s "Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$final_dmg_name"

echo "notorizing..."
echo
# submit to notarize
xcrun altool --notarize-app -f "$final_dmg_name" -t osx -u ${APPLE_ID} -p dqgu-cmmc-aumy-kqwe -primary-bundle-id $bundle_id



